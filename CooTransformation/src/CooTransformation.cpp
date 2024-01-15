#include<CooTransformation/CooTransformation.hpp>

#include <iostream>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <string>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstring>
#include <memory>
// #include <filesystem>

#include <SpiceUsr.h>


// namespace fs = std::filesystem;

enum LogType
{
    LOG,
    DEBUG,
    WARNING,
    ERROR
};

static std::string LogTypeStr[] =
{
    "Log",
    "Debug",
    "Warning",
    "Error"
};

static bool s_bLogConsole = true;
static std::unique_ptr<std::ofstream> s_logfile;



static void Log(LogType eLogType, const std::string& rsMsg)
{
    std::string sMsg = std::string("[") + LogTypeStr[static_cast<int>(eLogType)] + "]: " + rsMsg;

    if (s_bLogConsole)
    {
        std::cout << "CooTransformation: " << sMsg << std::endl;
    }
    if (s_logfile && s_logfile->is_open())
    {
        auto time = std::time(nullptr);
        auto tm = *std::localtime(&time);
        std::stringstream ss;
        ss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S.%z%Z");
        std::string timestamp = ss.str();

        *s_logfile << "[" + timestamp + "] " << sMsg << std::endl;
    }
}



static bool SpiceHasFailed()
{
    // should use failed_c(), but linking fails. return_c has nearly the same functionality
    // see ftp://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/req/error.html#Handling%20Errors%20Detected%20in%20Your%20Own%20Program
    return (return_c() == SPICETRUE);
}   // SpiceHasFailed()



static void SetSpiceErrorHandling(const char* szAction, const char* szDevice, const char* szFormat)
{
    SpiceChar szErrorAction[10];
    SpiceChar szErrorDevice[10];
    SpiceChar szErrorFormat[10];

    strcpy(szErrorAction, szAction);
    strcpy(szErrorDevice, szDevice);
    strcpy(szErrorFormat, szFormat);

    // Set the default error action
    erract_c(
        "SET",      // In: Operation "SET" means "update the error response action to the value indicated by action."
        10,         // In: Length of list for output
        szErrorAction    // In: The default error action 
    );

    // Retrieve or set the name of the current output device for error messages
    errdev_c(
        "SET",      // In: Operation "SET" means "set the name of the current error output device equal to the value of device."
        10,         // In: Length of device for output
        szErrorDevice    // In: The device name. Screen or file name.
    );

    // Retrieve or set the list of error message items to be output when an error is detected
    errprt_c(
        "SET",      // In: Operation "SET" means "the following list specifies the default selection of error messages to be output."
        10,         // In: Length of list for output
        szErrorFormat    // In: Error message format
    );
}   // SetSpiceErrorHandling()


static bool IsDir(const std::string& rsDir)
{
    // to stay compatible with c++11 and not require boost

    struct stat oStat;
    if (!stat(rsDir.c_str(), &oStat) && oStat.st_mode & S_IFDIR)
    {
        return true;
    }
    return false;
}


static int LoadSpiceKernel(const std::string& pcSpiceKernelPath)
{
    if (!pcSpiceKernelPath.empty())
    {
        furnsh_c(pcSpiceKernelPath.c_str());
        if (SpiceHasFailed())
        {
            char acSMsg[SPICE_ERROR_SMSGLN]; // short message
            char acXMsg[SPICE_ERROR_XMSGLN]; // explanation of short message
            getmsg_c("SHORT", SPICE_ERROR_SMSGLN, acSMsg);
            getmsg_c("EXPLAIN", SPICE_ERROR_XMSGLN, acXMsg);
            reset_c();
            Log(LogType::WARNING,
                "Could not load CSPICE: \"" + pcSpiceKernelPath + "\" (" + acSMsg + ": " + acXMsg + ")!");
            return -1;
        }
        else
        {
            Log(LogType::LOG, "Loaded CSpice Kernel \"" + pcSpiceKernelPath + "\"");
        }
    }
    return 0;
}


JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
unsigned int GetDllVersion()
{
    return 2u;
}


JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
int Init(bool bConsoleLog, const char* pcLogDirectory)
{
    DeInit();
    s_bLogConsole = bConsoleLog;
    
    // init log file
    if(pcLogDirectory)
    {
        std::string oLogDir = pcLogDirectory;
        if(!oLogDir.empty())
        {
            if(IsDir(oLogDir))
            {
                s_logfile = std::make_unique<std::ofstream>(std::ofstream(oLogDir + "/CooTransformation.log", std::ios::out|std::ios::app));
                if(s_logfile->is_open())
                {
                    // s_bLogConsole = true;
                    Log(LogType::DEBUG, "Initialized file logger \"" + oLogDir + "/CooTransformation.log\"");
                    // s_bLogConsole = bConsoleLog;
                }
                else
                {
                    // s_bLogConsole = true;
                    Log(LogType::WARNING, "Could not open log file \"" + oLogDir + "/CooTransformation.log\"!");
                    // s_bLogConsole = bConsoleLog;
                }
            }
            else
            {
                // s_bLogConsole = true;
                Log(LogType::WARNING, "Log directory \"" + oLogDir + "\" does not exist!");
                // s_bLogConsole = bConsoleLog;
            }
        }
    }

    // Initialize CSPICE
    std::string sSpiceVersion(tkvrsn_c("toolkit"));
    Log(LogType::LOG, "Initializing SPICE version '" + sSpiceVersion + "'.");
    // Set error handling system of CSPICE to continue if error occurs
    SetSpiceErrorHandling("RETURN", "NULL", "SHORT");
    return 0;
}

JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
int AddSpiceKernel(const char* pcKernelPath)
{
    return LoadSpiceKernel(pcKernelPath);
}


JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
void DeInit()
{
    s_logfile.reset();
}


JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
int Xyz2LatLonRad(double dX, double dY, double dZ, double* pdLat, double* pdLon, double* pdRad)
{
    double adXyz[3] = { dX * 0.001, dY * 0.001, dZ * 0.001 };
    reclat_c(adXyz, pdRad, pdLon, pdLat);
    if (SpiceHasFailed())
    {
        reset_c();
        return -1;
    }
    // Convert to degree and meters
    *pdLon /= rpd_c();
    *pdLat /= rpd_c();
    *pdRad *= 1000.0;
    return 0;
}


JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
int Xyz2LatLonAlt(const char* pcPlanet, double dX, double dY, double dZ, double* pdLat, double* pdLon, double* pdAlt)
{
    // Look up the radii for the planet. Although we omit it here, we could first call badkpv_c
    // to make sure the variable BODY?99_RADII has three elements and numeric data type.
    // If the variable is not present in the kernel pool, bodvrd_c will signal an error.
    SpiceInt nDim = 0;
    double adRadii[3] = { 0.0, 0.0, 0.0 };
    bodvrd_c(pcPlanet, "RADII", 3, &nDim, adRadii);
    if (SpiceHasFailed() || nDim != 3)
    {
        reset_c();
        return -2;
    }

    // Compute flattening coefficient.
    double dRadiusEquat = adRadii[0];
    double dRadiusPole = dRadiusEquat; // use spherical Mars model instead of spheroid to be consistent with MCZ conventions
    double dFlattening = (dRadiusEquat - dRadiusPole) / dRadiusEquat;

    // Do the conversion.
    double adXyz[3] = { dX * 0.001, dY * 0.001, dZ * 0.001 };
    recpgr_c(pcPlanet, adXyz, dRadiusEquat, dFlattening, pdLon, pdLat, pdAlt);
    if (SpiceHasFailed())
    {
        reset_c();
        return -1;
    }
    // Convert to degree and meters
    *pdLon /= rpd_c();
    *pdLat /= rpd_c();
    *pdAlt *= 1000.0;
    return 0;
}


JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
int LatLonAlt2Xyz(const char* pcPlanet, double dLat, double dLon, double dAlt, double* pdX, double* pdY, double* pdZ)
{
    // Look up the radii for the planet. Although we omit it here, we could first call badkpv_c
    // to make sure the variable BODY?99_RADII has three elements and numeric data type.
    // If the variable is not present in the kernel pool, bodvrd_c will signal an error.
    SpiceInt nDim = 0;
    double adRadii[3] = { 0.0, 0.0, 0.0 };
    bodvrd_c(pcPlanet, "RADII", 3, &nDim, adRadii);
    if (SpiceHasFailed() || nDim != 3)
    {
        reset_c();
        return -2;
    }

    // Compute flattening coefficient.
    double dRadiusEquat = adRadii[0];
    double dRadiusPole = dRadiusEquat; // use spherical Mars model instead of spheroid to be consistent with MCZ conventions
    double dFlattening = (dRadiusEquat - dRadiusPole) / dRadiusEquat;

    // Do the conversion.
    double adXyz[3] = { 0.0, 0.0, 0.0 };
    pgrrec_c(pcPlanet, dLon * rpd_c(), dLat * rpd_c(), dAlt * 0.001, dRadiusEquat, dFlattening, adXyz);
    if (SpiceHasFailed())
    {
        reset_c();
        return -1;
    }
    // Convert to meters
    *pdX = adXyz[0] * 1000.0;
    *pdY = adXyz[1] * 1000.0;
    *pdZ = adXyz[2] * 1000.0;
    return 0;
}

JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
int GetRelState(
    const char* pcTargetBody,
    const char* pcReferenceFrame,
    const char* pcAberrationCorrection,
    const char* pcObserverBody,
    double dObserverTime,
    double* pdLightTime,
    double* pdPosX,
    double* pdPosY,
    double* pdPosZ,
    double* pdVelX,
    double* pdVelY,
    double* pdVelZ
)
{
    //Return the state (position and velocity) of a target body
    //    relative to an observing body, optionally corrected for light
    //    time (planetary aberration) and stellar aberration.

    auto sAberrationCorrection = std::string{"NONE"};

    if ( pcAberrationCorrection != nullptr )
    {
        sAberrationCorrection = pcAberrationCorrection;
    }


    SpiceDouble state[6] = {};
    spkezr_c( pcTargetBody, dObserverTime, pcReferenceFrame, sAberrationCorrection.c_str(), pcObserverBody, state, pdLightTime );
    if(SpiceHasFailed() )
    {
        reset_c();
        return -1;
    }

    *pdPosX = state[0];
    *pdPosY = state[1];
    *pdPosZ = state[2];
    *pdVelX = state[3];
    *pdVelY = state[4];
    *pdVelZ = state[5];

    return 0;
}
