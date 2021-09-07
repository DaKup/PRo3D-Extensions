#include<CooTransformation/CooTransformation.hpp>

#include <iostream>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <string>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
//#include <filesystem>


#include <SpiceUsr.h>


//namespace fs = std::filesystem;

enum class LogType
{
    LOG,
    DEBUG,
    WARNING,
    FATAL
};

static std::string LogTypeStr[] =
{
    "Log",
    "Debug",
    "Warning",
    "Error"
};

static std::unique_ptr<std::ofstream> s_logfile;



static void _Log(LogType eLogType, const std::string& rsMsg)
{
    std::string sMsg = std::string("[") + LogTypeStr[static_cast<int>(eLogType)] + "]: " + rsMsg;
    std::cout << "CooTransformation: " << sMsg << std::endl;

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



static bool _SpiceHasFailed()
{
    // should use failed_c(), but linking fails. return_c has nearly the same functionality
    // see ftp://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/req/error.html#Handling%20Errors%20Detected%20in%20Your%20Own%20Program
    return (return_c() == SPICETRUE);
}   // _SpiceHasFailed()



static void _SetSpiceErrorHandling(const char* szAction, const char* szDevice, const char* szFormat)
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


static bool _IsDir(const std::string& rsDir)
{
    // to stay compatible with c++11 and not require boost

    struct stat oStat;
    if (!stat(rsDir.c_str(), &oStat) && oStat.st_mode & S_IFDIR)
    {
        return true;
    }
    return false;
}


static int _LoadSpiceKernel(const std::string& pcSpiceKernelPath)
{
    if (!pcSpiceKernelPath.empty())
    {
        furnsh_c(pcSpiceKernelPath.c_str());
        if (_SpiceHasFailed())
        {
            char acSMsg[SPICE_ERROR_SMSGLN]; // short message
            char acXMsg[SPICE_ERROR_XMSGLN]; // explanation of short message
            getmsg_c("SHORT", SPICE_ERROR_SMSGLN, acSMsg);
            getmsg_c("EXPLAIN", SPICE_ERROR_XMSGLN, acXMsg);
            reset_c();
            _Log(LogType::WARNING, "Could not load CSPICE: \"" + pcSpiceKernelPath + "\" (" + acSMsg + ": " + acXMsg + ")!");
            return -1;
        }
        else
        {
            std::cout << "Loaded CSpice Kernel \"" << pcSpiceKernelPath << "\"" << std::endl;
        }
    }
    return 0;
}





JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
unsigned int GetDllVersion()
{
    return 1u;
}


JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
int Init(const char* pcConfigDirectory, const char* pcLogDirectory)
{
    DeInit();

    _Log(LogType::DEBUG, "Initialized console logger");

     if( pcLogDirectory )
     {
         std::string oLogDir = pcLogDirectory;
         if( !oLogDir.empty() )
         {
             if( _IsDir( oLogDir ) )
             {
                 s_logfile = std::make_unique<std::ofstream>( std::ofstream(oLogDir + "/CooTransformation.log", std::ios::out|std::ios::app));
                 _Log(LogType::DEBUG, "Initialized file logger " + oLogDir + "/CooTransformation.log");
             }
             else
             {
                 s_logfile = std::make_unique<std::ofstream>(std::ofstream("CooTransformation.log", std::ios::out|std::ios::app));
                 _Log(LogType::DEBUG, "Initialized file logger ./CooTransformation.log");
             }
         }
     }

     if( !pcConfigDirectory || std::string( pcConfigDirectory ).empty() )
     {
         _Log(LogType::FATAL, "No pcConfigDirectory given");
         return -3;
     }

     std::string oConfigPath = pcConfigDirectory;

     if (oConfigPath.back() == '/' || oConfigPath.back() == '\\')
         oConfigPath.pop_back();
    
    if (!_IsDir(oConfigPath))
    {
        _Log(LogType::FATAL, "The given configuration directory " + oConfigPath + " doesn't exist");
        return -3;
    }

    // Initialize CSPICE
    std::string sSpiceVersion(tkvrsn_c("toolkit"));
    _Log(LogType::LOG, "Initializing SPICE version '" + sSpiceVersion + "'.");
    // Set error handling system of CSPICE to continue if error occurs
    _SetSpiceErrorHandling("RETURN", "NULL", "SHORT");


    auto oKernelList = std::ifstream(oConfigPath + "/kernel_list.txt", std::ios::in);
    int nKernelsLoaded = 0;
    if (oKernelList.is_open())
    {
        std::string sLine;
        while (std::getline(oKernelList, sLine))
        {
            if (sLine.empty()) continue;
            if (sLine.rfind(";", 0) == 0) continue;
            if (sLine.rfind("#", 0) == 0) continue;

            if (sLine.find(':') != std::string::npos || sLine.find('~') != std::string::npos)
            {
                // absolute path 
            }
            else
            {
                sLine = oConfigPath + "/" + sLine;
            }

            int nRet = _LoadSpiceKernel(sLine);
            if (nRet == 0)
            {
                nKernelsLoaded++;
            }
        }
    }
    else
    {
        _Log(LogType::FATAL, "Unable to read \"" + oConfigPath + "/kernel_list.txt\"");
        return -1;
    }

    if (nKernelsLoaded == 0)
    {
        _Log(LogType::WARNING, "No kernels were successfully loaded from \"" + oConfigPath + "/kernel_list.txt\"");
        return 0;
    }

    _Log(LogType::LOG, "Successfully loaded " + std::to_string(nKernelsLoaded) + " SPICE kernels from " + oConfigPath + "/kernel_list.txt" + ".");
    return 0;
}


JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
void DeInit()
{
    s_logfile.reset();
}


int Xyz2LatLonRad(double dX, double dY, double dZ, double* pdLat, double* pdLon, double* pdRad)
{
    double adXyz[3] = { dX * 0.001, dY * 0.001, dZ * 0.001 };
    reclat_c(adXyz, pdRad, pdLon, pdLat);
    if (_SpiceHasFailed())
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


int Xyz2LatLonAlt(const char* pcPlanet, double dX, double dY, double dZ, double* pdLat, double* pdLon, double* pdAlt)
{
    // Look up the radii for the planet. Although we omit it here, we could first call badkpv_c
    // to make sure the variable BODY?99_RADII has three elements and numeric data type.
    // If the variable is not present in the kernel pool, bodvrd_c will signal an error.
    SpiceInt nDim = 0;
    double adRadii[3] = { 0.0, 0.0, 0.0 };
    bodvrd_c(pcPlanet, "RADII", 3, &nDim, adRadii);
    if (_SpiceHasFailed() || nDim != 3)
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
    if (_SpiceHasFailed())
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


int LatLonAlt2Xyz(const char* pcPlanet, double dLat, double dLon, double dAlt, double* pdX, double* pdY, double* pdZ)
{
    // Look up the radii for the planet. Although we omit it here, we could first call badkpv_c
    // to make sure the variable BODY?99_RADII has three elements and numeric data type.
    // If the variable is not present in the kernel pool, bodvrd_c will signal an error.
    SpiceInt nDim = 0;
    double adRadii[3] = { 0.0, 0.0, 0.0 };
    bodvrd_c(pcPlanet, "RADII", 3, &nDim, adRadii);
    if (_SpiceHasFailed() || nDim != 3)
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
    if (_SpiceHasFailed())
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
