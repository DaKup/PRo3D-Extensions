#ifndef JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_HPP
#define JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_HPP

#include <CooTransformation/CooTransformationExport.hpp>

extern "C"
{
    /**
     * @brief Get API version.
     *
     * This function should be the first one to call.
     * Use it to validate the API version matches with your interface.
     * @return API version number.
     */
    JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
    unsigned int GetAPIVersion();

    /**
     * @brief Init logging.
     *
     * Enable console and/or file logging.
     * Log levels:
     * 0: Error
     * 1: Warning
     * 2: Info
     * 3: Debug
     * 4: Trace
     * @param[in] bConsoleLog       Enable or disable logging to stderr.
     * @param[in] pcLogFile         Optional path to log file. Can be NULL.
     * @param[in] nConsoleLogLevel  Console log level value [0, 5].
     * @param[in] nFileLogLevel     File log level value [0, 5].
     * @return
     *  0   Success
     */
    JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
    int Init(bool bConsoleLog, const char *pcLogFile, int nConsoleLogLevel, int nFileLogLevel);

    /**
     * @brief Flush log messages and close the log file.
     */
    JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
    void DeInit();

    /**
     * @brief Load an additional SPICE kernel.
     *
     * @param[in] pcSpiceKernelFile Path to a SPICE kernel file. This can be a meta-kernel file as well.
     * @return
     *  0   Success
     *  -1  Failed to load the kernel
     */
    JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
    int AddSpiceKernel(const char *pcSpiceKernelFile);

    /**
     * @brief Transform planet-centered cartesian coordinates to spherical coordinates.
     *
     * @param[in]   dX      X-coordinate in meters.
     * @param[in]   dY      Y-coordinate in meters.
     * @param[in]   dZ      Z-coordinate in degrees.
     * @param[out]  pdLat   Latitude in degrees.
     * @param[out]  pdLon   Longitude in degrees.
     * @param[out]  pdRad   Radius in meters.
     * @return
     *  0   Success
     * -1   Failed to transform coordinates.
     */
    JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
    int Xyz2LatLonRad(double dX, double dY, double dZ, double *pdLat, double *pdLon, double *pdRad);

    /**
     * @brief Transform planet-centered cartesian coordinates into planetographic coordinates.
     *
     * @param[in]   pcPlanet    Case-insensitive name of planet (eg. "mars" or "EARTH").
     * @param[in]   dX          X-coordinate in meters.
     * @param[in]   dY          Y-coordinate in meters.
     * @param[in]   dZ          Z-coordinate in meters.
     * @param[out]  pdLat       Latitude in degrees w.r.t. the referenced spheroid.
     * @param[out]  pdLon       Longitude in degrees w.r.t. the referenced spheroid.
     * @param[out]  pdAlt       Altitude in meters w.r.t. the referenced spheroid.
     * @return
     *  0   Success
     * -2   Failed to lookup radii of the planet
     * -3   Failed to transform coordinates
     */
    JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
    int Xyz2LatLonAlt(const char *pcPlanet, double dX, double dY, double dZ, double *pdLat, double *pdLon, double *pdAlt);

    /**
     * @brief Transform planetographic coordinates to planet-centered cartesian coordinates.
     *
     * @param[in]   pcPlanet    Case-insensitive name of planet (eg. "mars" or "EARTH")
     * @param[in]   dLat        Latitude in degrees w.r.t. the referenced spheroid
     * @param[in]   dLon        Longitude in degrees w.r.t. the referenced spheroid
     * @param[in]   dAlt        Altitude in meters w.r.t. the referenced spheroid
     * @param[out]  pdX         X-coordinate in meters
     * @param[out]  pdY         Y-coordinate in meters
     * @param[out]  pdZ         Z-coordinate in meters
     * @return
     *  0   Success
     * -1   Failed to lookup radii for the planet
     * -2   Failed to transform coordinates
     */
    JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
    int LatLonAlt2Xyz(const char *pcPlanet, double dLat, double dLon, double dAlt, double *pdX, double *pdY, double *pdZ);

    /**
     * @brief Get the relative position and rotation of a celestial body.
     *
     * @param[in]   pcTargetBody            Case-insensitive name of a celestial body (eg. "mars" or "EARTH")
     * @param[in]   pcSupportBody           Case-insensitive name of a celestial body (eg. "mars" or "EARTH")
     * @param[in]   pcObserverBody          Case-insensitive name of a celestial body (eg. "mars" or "EARTH")
     * @param[in]   pcObserverDatetime      Datetime string (e.g. "2026-12-03 08:15:00.00")
     * @param[in]   pcOutputReferenceFrame  Reference frame (e.g. "J2000")
     * @param[out]  pdPosVec                3x1 relative position in meters
     * @param[out]  pdRotMat                3x3 rotation matrix
     * @return
     *  0   Success
     * -1   Failed to convert datetime string format
     * -2   Failed to get relative state of support body w.r.t. observer body
     * -3   Failed to get relative state of target body w.r.t. observer body
     */
    JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
    int GetRelState(
        const char *pcTargetBody,
        const char *pcSupportBody,
        const char *pcObserverBody,
        const char *pcObserverDatetime,
        const char *pcOutputReferenceFrame,
        double *pdPosVec,
        double *pdRotMat);

    /**
     * @brief Return the matrix that transforms position vectors from one
     * specified frame to another at a specified epoch.
     * See also https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/cspice/pxform_c.html
     *
     * @param[in]   pcFrom      Name of the frame to transform from (e.g. "IAU_EARTH")
     * @param[in]   pcTo        Name of the frame to transform to (e.g. "J2000")
     * @param[in]   pcDatetime  Datetime string (e.g. "2026-12-03 08:15:00.00")
     * @param[out]  pdRotMat    3x3 rotation matrix
     * @return
     *  0   Success
     * -1   Failed to convert datetime string format
     */
    JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
    int GetPositionTransformationMatrix(
        const char *pcFrom,
        const char *pcTo,
        const char *pcDatetime,
        double *pdRotMat);

} // extern "C"

#endif // JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_HPP
