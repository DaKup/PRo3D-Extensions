#ifndef JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_HPP
#define JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_HPP

#include <CooTransformation/CooTransformationExport.hpp>

extern "C"
{
    ///**
    //  * GetAPIVersion returns the version number of the library's API.
    //  * This function can be called, before Init has been executed.
    //  * @returns api version number.
    //  **/
    JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
        unsigned int GetAPIVersion();

    /**
     * TODO: change log directory to a log file path
     * TODO: allow logging to stderr?
     * Initializes console and/or file logging.
     * 
     * @param[in] bConsoleLog: if true, log to stdout
     * @param[in] pcLogDir: optional path to a directory in which a CooTransformation.log file will be created
     * @return error code 
     */
     JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
         int Init(bool bConsoleLog, const char* pcLogFile, int nConsoleLogLevel, int nFileLogLevel );

     //JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
    //    int Init(bool bConsoleLog, const char* pcLogDir); 



    /** DeInit makes sure, that any remaining log messages in the buffer are written and all open files are closed.
      **/
    JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
        void DeInit();


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

    /**
     * TODO: Add proper description.
     * For more details see https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/cspice/spkezr_c.html
     * 
     * @param pcTargetBody 
     * @param pcReferenceFrame 
     * @param pcAberrationCorrection 
     * @param pcObserverBody 
     * @param dObserverTime 
     * @param pdLightTime 
     * @param pdPosX 
     * @param pdPosY 
     * @param pdPosZ 
     * @param pdVelX 
     * @param pdVelY 
     * @param pdVelZ 
     * @return error code
     */
    JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
    int GetRelState(
        const char* pcTargetBody,
        const char* pcSunBody,
        //const char* pcAberrationCorrection,
        const char* pcObserverBody,
        const char* pcObserverDatetime,
        const char* pcOutputReferenceFrame,
        //double dObserverTime,
        //double* pdLightTime,
        double* pdPosVec,
        double* pdRotMat
    );

    // todo: https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/cspice/pxform_c.html
    JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_EXPORT
    int GetPositionTransformationMatrix(
        const char* pcFrom,
        const char* pcTo,
        const char* pcDatetime,
        double* pdRotMat
    );

} // extern "C"

#endif // JR_PRO3D_EXTENSIONS_COOTRANSFORMATION_HPP
