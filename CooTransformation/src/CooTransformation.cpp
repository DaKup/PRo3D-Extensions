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
#include <array>
#include <vector>
#include <algorithm>
#include <type_traits>
// #include <filesystem>

#include <SpiceUsr.h>


// namespace fs = std::filesystem;

enum LogLevel
{
    ERROR,
    WARNING,
    INFO,
    DEBUG,
    TRACE
};

static std::string LogLevelStr[] =
{
    "Error",
    "Warning",
    "Debug",
    "Info",
    "Trace"
};

static int s_nConsoleLogLevel = static_cast<int>(TRACE);
static int s_nFileLogLevel = static_cast<int>(TRACE);
static bool s_bLogConsole = true;
static std::unique_ptr<std::ofstream> s_logfile;



static void Log(LogLevel eLogType, const std::string& rsMsg)
{
    //std::cout << "Log() called with eLogType = " << std::to_string( static_cast<int>( eLogType ) ) << ", console log level = " << std::to_string( s_nConsoleLogLevel ) << std::endl;

    std::string sMsg = std::string("[") + LogLevelStr[static_cast<int>(eLogType)] + "]: " + rsMsg;

    if (s_bLogConsole && s_nConsoleLogLevel >= static_cast<int>(eLogType))
    {
        //std::cout << "CooTransformation: " << sMsg << std::endl;
        std::cerr << "CooTransformation: " << sMsg << std::endl;
    }
    if (s_logfile && s_logfile->is_open() && s_nFileLogLevel >= static_cast<int>(eLogType))
    {
        auto time = std::time(nullptr);
        auto tm = *std::localtime(&time);
        std::stringstream ss;
        ss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S.%z%Z");
        std::string timestamp = ss.str();

        *s_logfile << "[" + timestamp + "] " << sMsg << std::endl;
        s_logfile->flush();
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
    SpiceChar szErrorAction[SPICE_ERROR_LMSGLN];
    SpiceChar szErrorDevice[SPICE_ERROR_LMSGLN];
    SpiceChar szErrorFormat[SPICE_ERROR_LMSGLN];

    strcpy(szErrorAction, szAction);
    strcpy(szErrorDevice, szDevice);
    strcpy(szErrorFormat, szFormat);

    // Set the default error action
    erract_c(
        "SET",      // In: Operation "SET" means "update the error response action to the value indicated by action."
        SPICE_ERROR_LMSGLN,         // In: Length of list for output
        szErrorAction    // In: The default error action 
    );

    // Retrieve or set the name of the current output device for error messages
    errdev_c(
        "SET",      // In: Operation "SET" means "set the name of the current error output device equal to the value of device."
        SPICE_ERROR_LMSGLN,         // In: Length of device for output
        szErrorDevice    // In: The device name. Screen or file name.
    );

    // Retrieve or set the list of error message items to be output when an error is detected
    errprt_c(
        "SET",      // In: Operation "SET" means "the following list specifies the default selection of error messages to be output."
        SPICE_ERROR_LMSGLN,         // In: Length of list for output
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
    Log(LogLevel::TRACE, "LoadSpiceKernel() called with spice kernel path = \"" + pcSpiceKernelPath + "\".");
    if (!pcSpiceKernelPath.empty())
    {
        furnsh_c(pcSpiceKernelPath.c_str());
        if (SpiceHasFailed())
        {
            char acSMsg[SPICE_ERROR_LMSGLN]; // short message
            char acXMsg[SPICE_ERROR_LMSGLN]; // explanation of short message
            getmsg_c("SHORT", SPICE_ERROR_LMSGLN, acSMsg);
            getmsg_c("EXPLAIN", SPICE_ERROR_LMSGLN, acXMsg);
            reset_c();
            Log(LogLevel::WARNING,
                "Could not load CSPICE: \"" + pcSpiceKernelPath + "\" (" + acSMsg + ": " + acXMsg + ")!");
            return -1;
        }
        else
        {
            Log(LogLevel::INFO, "Loaded CSpice Kernel \"" + pcSpiceKernelPath + "\".");
        }
    }
    return 0;
}


JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
int Str2Et( const std::string& rsTimestamp, double& rdEt )
{
    Log(LogLevel::TRACE, "Str2Et() called with timestamp = \"" + rsTimestamp + "\".");

    str2et_c( rsTimestamp.c_str(), &rdEt );
    if( SpiceHasFailed() )
    {
        char acSMsg[SPICE_ERROR_LMSGLN]; // short message
        char acXMsg[SPICE_ERROR_LMSGLN]; // explanation of short message
        getmsg_c("SHORT", SPICE_ERROR_LMSGLN, acSMsg);
        getmsg_c("EXPLAIN", SPICE_ERROR_LMSGLN, acXMsg);
        reset_c();
        Log(LogLevel::WARNING,
            std::string("Str2Et() failed with error: \"") + acSMsg + "\": \"" + acXMsg + "\"");
        return -1;
    }

    Log(LogLevel::TRACE, "Str2Et() finished with et = " + std::to_string( rdEt ));
    return 0;
}


JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
unsigned int GetDllVersion()
{
    Log(LogLevel::TRACE, "GetDllVersion() called.");
    Log(LogLevel::TRACE, "GetDllVersion() finished.");
    return 3;
}


JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
int Init(bool bConsoleLog, const char* pcLogFile, int nConsoleLogLevel, int nFileLogLevel)
{
    Log(LogLevel::TRACE, "Init() called.");
    DeInit();

    // temporarily enable logging:
    s_nConsoleLogLevel = static_cast<int>(TRACE);
    s_nFileLogLevel = static_cast<int>(TRACE);
    s_bLogConsole = true;

    // clamp log levels:
    nConsoleLogLevel = std::clamp( nConsoleLogLevel, -1, static_cast<int>(LogLevel::TRACE));
    nFileLogLevel = std::clamp( nConsoleLogLevel, -1, static_cast<int>(LogLevel::TRACE));

    if ( nConsoleLogLevel >= 0 )
    {
        Log(LogLevel::TRACE, "Console log level set to \"" + LogLevelStr[nConsoleLogLevel] + "\"." );
    }
    else
    {
        s_bLogConsole = false;
    }

    s_bLogConsole = bConsoleLog;
    s_nConsoleLogLevel = nConsoleLogLevel;
    s_nFileLogLevel = nFileLogLevel;


    if( pcLogFile )
    {
        std::string sLogFile = pcLogFile;
        if(!sLogFile.empty())
        {
            s_logfile = std::make_unique<std::ofstream>(std::ofstream(sLogFile, std::ios::out|std::ios::app));
            if(s_logfile->is_open())
            {
                Log(LogLevel::DEBUG, "Initialized log file: \"" + sLogFile + "\"");
            }
            else
            {
                Log(LogLevel::WARNING, "Could not open log file: \"" + sLogFile + "\"!");
            }
        }
    }

    // Initialize CSPICE library
    std::string sSpiceVersion(tkvrsn_c("toolkit"));
    Log(LogLevel::INFO, "Initializing SPICE toolkit version '" + sSpiceVersion + "'.");

    // Set error handling system of CSPICE to continue if error occurs
    SetSpiceErrorHandling("RETURN", "NULL", "SHORT, EXPLAIN");
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
    Log(LogLevel::TRACE, "Xyz2LatLonRad() called with xyz = (" + std::to_string(dX) + ", " + std::to_string(dY) + ", " + std::to_string(dZ) + ")");
    
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

    Log(LogLevel::TRACE, "Xyz2LatLonRad() finished with lat, lon, rad = (" + std::to_string(*pdLat) + ", " + std::to_string(*pdLon) + ", " + std::to_string(*pdRad) + ")");

    return 0;
}


JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
int Xyz2LatLonAlt(const char* pcPlanet, double dX, double dY, double dZ, double* pdLat, double* pdLon, double* pdAlt)
{
    Log(LogLevel::TRACE, "Xyz2LatLonAlt() called with planet = " + std::string{pcPlanet} + ", xyz = (" + std::to_string(dX) + ", " + std::to_string(dY) + ", " + std::to_string(dZ) + ")");

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

    Log(LogLevel::TRACE, "Xyz2LatLonAlt() finished with lat, lon, alt = (" + std::to_string(*pdLat) + ", " + std::to_string(*pdLon) + ", " + std::to_string(*pdAlt) + ")");

    return 0;
}


JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
int LatLonAlt2Xyz(const char* pcPlanet, double dLat, double dLon, double dAlt, double* pdX, double* pdY, double* pdZ)
{
    Log(LogLevel::TRACE, "LatLonAlt2Xyz() called with planet = " + std::string{pcPlanet} + ", xyz = (" + std::to_string(dLat) + ", " + std::to_string(dLon) + ", " + std::to_string(dAlt) + ")");
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

    Log(LogLevel::TRACE, "LatLonAlt2Xyz() finished with xyz = (" + std::to_string(*pdX) + ", " + std::to_string(*pdY) + ", " + std::to_string(*pdZ) + ")");
    return 0;
}


JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
int GetRelState(
    const char* pcTargetBody,
    const char* pcSupportBody,
    //const char* pcAberrationCorrection,
    const char* pcObserverBody,
    const char* pcObserverTime,
    const char* pcOutputReferenceFrame,
    //double dObserverTime,
    //double* pdLightTime,
    //double* pdPosX,
    //double* pdPosY,
    //double* pdPosZ,
    double* pdPosVec,
    //double* pdVelX,
    //double* pdVelY,
    //double* pdVelZ
    double* pdRotMat
)
{
    Log(LogLevel::TRACE, std::string{"GetRelState() called with "} +
        "target body = \"" + std::string{pcTargetBody} + "\" " +
        "support body = \"" + std::string{pcSupportBody} + "\" " +
        "observer body = \"" + std::string{pcObserverBody} + "\" " +
        "observer time = \"" + std::string{pcObserverTime} + "\" " +
        "reference frame = \"" + std::string{pcOutputReferenceFrame} + "\"."
    );

    //Return the state (position and velocity) of a target body
    //    relative to an observing body, optionally corrected for light
    //    time (planetary aberration) and stellar aberration.

    double dObserverTime = {};
    int ret_val = Str2Et( pcObserverTime, dObserverTime );
    if(ret_val != 0)
    {
        return ret_val;
    }

    auto sAberrationCorrection = std::string{"NONE"};
    //if ( pcAberrationCorrection != nullptr )
    //{
    //    sAberrationCorrection = pcAberrationCorrection;
    //}


    // Support body position for rotation, e.g. SUN:
    auto dSupportPosVec = std::array<double, 3>{};
    {
        //SpiceDouble state[6] = {};
        auto state = std::array<double, 6>{};
        double unused = {};
        spkezr_c( pcSupportBody, dObserverTime, pcOutputReferenceFrame, sAberrationCorrection.c_str(), pcObserverBody, state.data(), &unused );
        if(SpiceHasFailed())
        {
            reset_c();
            return -2;
        }

        dSupportPosVec[0] = state[0];
        dSupportPosVec[1] = state[1];
        dSupportPosVec[2] = state[2];
    }

    // #1.-position of mars (target) from HERA (Observr) in ECLIPJ2000--> spkezr
    // Target Position, e.g. Hera:
    {
        SpiceDouble state[6] = {};
        double unused = {};
        spkezr_c( pcTargetBody, dObserverTime, pcOutputReferenceFrame, sAberrationCorrection.c_str(), pcObserverBody, state, &unused );
        if(SpiceHasFailed())
        {
            reset_c();
            return -2;
        }

        pdPosVec[0] = state[0];
        pdPosVec[1] = state[1];
        pdPosVec[2] = state[2];
    }

    // Rotation:
    {
        //double dSunPos[3] = {dSunPosX, dSunPosY, dSunPosZ};
        //double dHeraPos[3] = {*pdPosX, *pdPosY, *pdPosZ};
        double dSunPosNorm[3] = {};
        double dHeraPosNorm[3] = {};
        double dUnused[3] = {};

        // #4.- normalize HERA-SUn vector (uSOL)
        // uSOL = spice.unorm(sunPos)[0]
        unorm_c( dSupportPosVec.data(), dSunPosNorm, dUnused );


        // #2.-normalize HERA-MARS vector -->uz
        // uz = spice.unorm(MarsPos)[0]
        unorm_c( pdPosVec, dHeraPosNorm, dUnused );
        //double dUZ[3] = {-pdPosVec[0], -pdPosVec[1], -pdPosVec[2]};
        double dUZ[3] = {dHeraPosNorm[0], dHeraPosNorm[1], dHeraPosNorm[2]};
        double dUY[3];
        double dUX[3];

        // #5.- multiply uz x uSOL = vy
        // vy = spice.vcrss(uz, uSOL)
        vcrss_c( dUZ, dSunPosNorm, dUY );

        // #6.-normalize vy (=uy)
        // uy= spice.unorm(vy)[0]
        double dTmp[3];
        unorm_c( dUY, dTmp, dUnused );
        dUY[0] = dTmp[0];
        dUY[1] = dTmp[1];
        dUY[2] = dTmp[2];

        // #7.- multiply uz x uy (= vx)
        //vx = spice.vcrss(uy, uz)
        vcrss_c( dUY, dUZ, dUX );


        // #8.- normalize vx (= ux)
        // ux = spice.unorm(vx)[0]
        unorm_c( dUX, dTmp, dUnused );
        dUX[0] = dTmp[0];
        dUX[1] = dTmp[1];
        dUX[2] = dTmp[2];

        // #9.-compose rotation matrix
        // np.array([ux, uy, uz])
        // compose:
        pdRotMat[0] = dUX[0];
        pdRotMat[1] = dUX[1];
        pdRotMat[2] = dUX[2];

        pdRotMat[3] = dUY[0];
        pdRotMat[4] = dUY[1];
        pdRotMat[5] = dUY[2];

        pdRotMat[6] = dUZ[0];
        pdRotMat[7] = dUZ[1];
        pdRotMat[8] = dUZ[2];
        
        //// compose + transpose:
        //pdRotMat[0] = dUX[0];
        //pdRotMat[1] = dUY[0];
        //pdRotMat[2] = dUZ[0];

        //pdRotMat[3] = dUX[1];
        //pdRotMat[4] = dUY[1];
        //pdRotMat[5] = dUZ[1];

        //pdRotMat[6] = dUX[2];
        //pdRotMat[7] = dUY[2];
        //pdRotMat[8] = dUZ[2];
    }

    // [km] to [m];
    pdPosVec[0] *= 1000;
    pdPosVec[1] *= 1000;
    pdPosVec[2] *= 1000;

    Log(LogLevel::TRACE, std::string{"GetRelState() finished with "} +
        "pos = (" + std::to_string(pdPosVec[0]) + ", " + std::to_string(pdPosVec[1]) + ", " + std::to_string(pdPosVec[2]) + "), " +
        "rot = (" + std::to_string(pdRotMat[0]) + ", " + std::to_string(pdRotMat[1]) + ", " + std::to_string(pdRotMat[2]) + ", " +
        std::to_string(pdRotMat[3]) + ", " + std::to_string(pdRotMat[4]) + ", " + std::to_string(pdRotMat[5]) + ", " +
        std::to_string(pdRotMat[6]) + ", " + std::to_string(pdRotMat[7]) + ", " + std::to_string(pdRotMat[8]) + ")."
    );

    return 0;
}


JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
int GetPositionTransformationMatrix(
    const char* pcFrom,
    const char* pcTo,
    const char* pcDatetime,
    double* pdRotMat
)
{
    Log(LogLevel::TRACE, "GetPositionTransformationMatrix() called with from = \"" + std::string{pcFrom} + "\", to = \"" + std::string{pcTo} + "\".");

    double dEt;
    int ret_val = Str2Et( pcDatetime, dEt );
    if(ret_val != 0)
    {
        return ret_val;
    }

    double dTmp[3][3];
    pxform_c( pcFrom, pcTo, dEt, dTmp);

    // reshape:
    pdRotMat[0] = dTmp[0][0];
    pdRotMat[1] = dTmp[0][1];
    pdRotMat[2] = dTmp[0][2];

    pdRotMat[3] = dTmp[1][0];
    pdRotMat[4] = dTmp[1][1];
    pdRotMat[5] = dTmp[1][2];

    pdRotMat[6] = dTmp[2][0];
    pdRotMat[7] = dTmp[2][1];
    pdRotMat[8] = dTmp[2][2];

    Log(LogLevel::TRACE, std::string{"GetPositionTransformationMatrix() finished with "} +
        "rot = (" + std::to_string(pdRotMat[0]) + ", " + std::to_string(pdRotMat[1]) + ", " + std::to_string(pdRotMat[2]) + ", " +
        std::to_string(pdRotMat[3]) + ", " + std::to_string(pdRotMat[4]) + ", " + std::to_string(pdRotMat[5]) + ", " +
        std::to_string(pdRotMat[6]) + ", " + std::to_string(pdRotMat[7]) + ", " + std::to_string(pdRotMat[8]) + ")."
    );

    return 0;
}
