#include<CooTransformation/CooTransformation.hpp>

#include <boost/filesystem.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

#include <cspice/SpiceUsr.h>


namespace bfs = boost::filesystem;


static boost::shared_ptr< boost::log::sinks::synchronous_sink<boost::log::sinks::text_ostream_backend> > s_console_sink;
static boost::shared_ptr< boost::log::sinks::synchronous_sink<boost::log::sinks::text_file_backend> > s_file_sink;


bool _SpiceHasFailed()
{
    // should use failed_c(), but linking fails. return_c has nearly the same functionality
    // see ftp://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/req/error.html#Handling%20Errors%20Detected%20in%20Your%20Own%20Program
    return (return_c() == SPICETRUE);
}   // _SpiceHasFailed()


void _SetSpiceErrorHandling(const char* szAction, const char* szDevice, const char* szFormat)
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


JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
unsigned int GetDllVersion()
{
    return 1u;
}


JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
int Init(const char* pcConfigDirectory, const char* pcLogDirectory)
{
    DeInit();
    std::string sLogFormat = "[%TimeStamp%][%Severity%]:  %Message%";
    boost::log::register_simple_formatter_factory< boost::log::trivial::severity_level, char >("Severity");
    boost::log::add_common_attributes();

    s_console_sink = boost::log::add_console_log(
        std::clog,
        boost::log::keywords::format = sLogFormat,
        boost::log::keywords::auto_flush = true
    );

    BOOST_LOG_TRIVIAL(trace) << "Initialized console logger";

     if( pcLogDirectory )
     {
         bfs::path oLogDir( pcLogDirectory );
         if( !oLogDir.empty() )
         {
             oLogDir = oLogDir.native();
             if( !bfs::exists( oLogDir ) )
             {
                 //BOOST_LOG_TRIVIAL(fatal) << "The given log directory " << pcLogDirectory << " doesn't exist";
                 //return -2;
                 s_file_sink = boost::log::add_file_log(
                     boost::log::keywords::file_name = "CooTransformation.log",
                     boost::log::keywords::format = sLogFormat,
                     boost::log::keywords::auto_flush = true
                 );
                 BOOST_LOG_TRIVIAL(trace) << "Initialized file logger CooTransformation.log";
             }
             else
             {
                 s_file_sink = boost::log::add_file_log(
                     boost::log::keywords::file_name = oLogDir.string() + "/CooTransformation.log",
                     boost::log::keywords::format = sLogFormat,
                     boost::log::keywords::auto_flush = true
                 );
                 BOOST_LOG_TRIVIAL(trace) << "Initialized file logger " << oLogDir.string() + "/CooTransformation.log";
             }
         }
     }

     if( !pcConfigDirectory || std::string( pcConfigDirectory ) == "" )
     {
         BOOST_LOG_TRIVIAL(fatal) << "No pcConfigDirectory given";
         return -3;
     }

    bfs::path oConfigPath(pcConfigDirectory);
    oConfigPath = oConfigPath.native();
    if (!bfs::exists(oConfigPath))
    {
        BOOST_LOG_TRIVIAL(fatal) << "The given configuration directory " << oConfigPath.string() << " doesn't exist";
        return -3;
    }

    // Initialize CSPICE
    std::string sSpiceVersion(tkvrsn_c("toolkit"));
    BOOST_LOG_TRIVIAL(info) << "Initializing SPICE version '" << sSpiceVersion << "'.";
    // Set error handling system of CSPICE to continue if error occurs
    _SetSpiceErrorHandling("RETURN", "NULL", "SHORT");
    // TODO: Load all spice kernels in config directory!
    bfs::path oConfigFile(oConfigPath);
    oConfigFile /= "pck00010.tpc";
    furnsh_c(oConfigFile.string().c_str());
    if (_SpiceHasFailed())
    {
        char acSMsg[SPICE_ERROR_SMSGLN]; // short message
        char acXMsg[SPICE_ERROR_XMSGLN]; // explanation of short message
        getmsg_c("SHORT", SPICE_ERROR_SMSGLN, acSMsg);
        getmsg_c("EXPLAIN", SPICE_ERROR_XMSGLN, acXMsg);
        reset_c();
        BOOST_LOG_TRIVIAL(fatal) << "Could not initialize CSPICE (" << acSMsg << ": " << acXMsg << ")!";
        return -1;
    }

    BOOST_LOG_TRIVIAL(info) << "Successfully loaded SPICE kernels from " << oConfigPath.string() << ".";
    return 0;
}


JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
void DeInit()
{
    if (s_console_sink)
    {
        s_console_sink->flush();
        boost::log::core::get()->remove_sink(s_console_sink);
        s_console_sink.reset();
    }
    if (s_file_sink)
    {
        s_file_sink->flush();
        boost::log::core::get()->remove_sink(s_file_sink);
        s_file_sink.reset();
    }
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
