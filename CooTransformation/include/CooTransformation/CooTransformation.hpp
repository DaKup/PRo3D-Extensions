#ifndef JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_HPP
#define JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_HPP

#include <CooTransformation/CooTransformationExport.hpp>

extern "C"
{
    /** GetDllVersion returns the version number of the DLL.
      * This function can be called, before Init has been executed.
      * @returns dll version number.
      **/
    JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
        unsigned int GetDllVersion();

    // /** Init reads all configuration files in the given directory and initializes internal
    //   * structures.
    //   * After successful execution of Init, the library is ready to be used.
    //   * @param[in] pcConfigDirectory: directory that contains the config files.
    //   * @param[in] pcLogDirectory: directory for the log file.
    //   * @returns error code.
    //   **/
    // JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
    //     int Init(const char* pcConfigDirectory, const char* pcLogDirectory);

    /** DeInit makes sure, that any remaining log messages in the buffer are written and all open files are closed.
      **/
    JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
        void DeInit();

    JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
        int Init(bool bConsoleLog, const char* pcLogFile);

    JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
        int AddSpiceKernel(const char* pcSpiceKernelFile);

    /** Xyz2LatLonRad simply transforms planet-centered cartesian coordinates into spherical coordinates.
      * Before this function can be called, Init has to be executed.
      * @param[in] dX, dY, dZ: cartesian coordinates.
      * @param[out] pdLat, pdLon, pdRad: spherical coordinates.
      * @returns error code.
      **/
    JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
        int Xyz2LatLonRad(double dX, double dY, double dZ, double* pdLat, double* pdLon, double* pdRad);

    /** Xyz2LatLonAlt transforms planet-centered cartesian coordinates into planetographic coordinates.
      * Before this function can be called, Init has to be executed.
      * @param[in] pcPlanet: case-insensitive name of planet (eg. "mars" or "EARTH").
      * @param[in] dX, dY, dZ: cartesian coordinates.
      * @param[out] pdLat, pdLon, pdAlt: latitude, longitude and altitude with respect to the reference spheroid.
      * @returns error code.
      **/
    JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
        int Xyz2LatLonAlt(const char* pcPlanet, double dX, double dY, double dZ, double* pdLat, double* pdLon, double* pdAlt);

    /** LatLonAlt2Xyz is the inverse of Xyz2LatLonAlt.
      * Before this function can be called, Init has to be executed.
      * @param[in] pcPlanet: case-insensitive name of planet (eg. "mars" or "EARTH").
      * @param[in] dLat, dLon, dAlt: latitude, longitude and altitude with respect to the reference spheroid.
      * @param[out] pdX, pdY, pdZ: cartesian coordinates.
      * @returns error code.
      **/
    JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
        int LatLonAlt2Xyz(const char* pcPlanet, double dLat, double dLon, double dAlt, double* pdX, double* pdY, double* pdZ);

    /** TODO Description
      **/
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
    );

} // extern "C"

#endif // JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_HPP
