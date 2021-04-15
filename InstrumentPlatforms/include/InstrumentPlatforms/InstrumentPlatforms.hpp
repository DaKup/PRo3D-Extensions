#ifndef JR_PRO3D_EXTENSIONS_INSTRUMENTPLATFORMS_HPP
#define JR_PRO3D_EXTENSIONS_INSTRUMENTPLATFORMS_HPP

#include <InstrumentPlatforms/InstrumentPlatformsExport.hpp>

/** InstrumentPlatforms Function Interface
* ======================================
*
* GENERAL INFORMATION
* All orientations (instrument, axes) provided by JR and held in SPlatform are given in ground
* reference frame.
*
* The ground reference frame (GRF) is per definition a right hand coordinate system, located at
* a user selected point on the ground:
*     +XGRF is set to be identical to the (user defined) platform LookAt vector (point click on
*     the ground);
*     +ZGRF is defined as up vector perpendicular to a plane defined by a given number of
*     platform points touching the ground;
*     +YGRF completes the right hand system;
*
* The platform reference frame (PRF) can be any arbitrary right hand coordinate system. The
* transformation Platform2Ground is to be provided by JR.
*
* The surface reference frame (SRF) can be any arbitrary right hand coordinate system. The
* transformation Ground2Surface is to be provided by VRVis.
*
**/


extern "C"
{


    /** SPoint3D: 3D point coordinates with double precision. **/
    struct JR_PRO3D_EXTENSIONS_INSTRUMENTPLATFORMS_EXPORT SPoint3D
    {
        double m_dX;
        double m_dY;
        double m_dZ;
    };


    /** SVector3D: 3D vector with double precision. **/
    struct JR_PRO3D_EXTENSIONS_INSTRUMENTPLATFORMS_EXPORT SVector3D
    {
        double m_dX;
        double m_dY;
        double m_dZ;
    };


    /** SAxis: used to define the degrees of freedom for various instruments.
      * Start and end point define a right hand system for rotation.
      **/
    struct JR_PRO3D_EXTENSIONS_INSTRUMENTPLATFORMS_EXPORT SAxis
    {
        const char* m_pcAxisId;
        const char* m_pcAxisDescription;
        SPoint3D m_oStartPoint;
        SPoint3D m_oEndPoint;
        double m_dMinAngle; /* unit: gon */
        double m_dMaxAngle; /* unit: gon */
        double m_dCurrentAngle; /* unit: gon */
    };


    /** SBoundingBox: used to define any Bounding Box dimensions/orientation in ground
      * reference frame.
      * Bounding boxes may serve as simplified models for visualization purposes.
      * The bounding box is defined by one corner point and the 3 edges which start from this
      * point. All other corner points can be calculated by simple addition of the origin with
      * 1 - 3 edge vectors.
      **/
    struct JR_PRO3D_EXTENSIONS_INSTRUMENTPLATFORMS_EXPORT SBoundingBox
    {
        SPoint3D m_oOriginBB;
        /* indicating direction and length of a bounding box's edge */
        SVector3D m_oEdge1;
        SVector3D m_oEdge2;
        SVector3D m_oEdge3;
    };


    /** STransformationMatrix: 4x4 matrix (Helmert transformation matrix).
      * Per definition STransformationMatrix defines a right hand system, with LookAt vector =
      * positive orientation of x-axis and Up vector = positive direction of z-axis.
      **/
    struct JR_PRO3D_EXTENSIONS_INSTRUMENTPLATFORMS_EXPORT STransformationMatrix
    {
        double m_adElement[4][4];
    };


    /** STransformation: transformation (STransformationMatrix) between source and target
      * reference frame.
      **/
    struct JR_PRO3D_EXTENSIONS_INSTRUMENTPLATFORMS_EXPORT STransformation
    {
        const char* m_pcTransfName;
        const char* m_pcSourceFrame;
        const char* m_pcTargetFrame;
        STransformationMatrix m_oHelmertTransfMatrix;
    };


    /** SInstrumentIntrinsics: instrument intrinsic parameters.
      * some of the given members may appear to be redundant; VRVis to decide which to be used.
      **/
    struct JR_PRO3D_EXTENSIONS_INSTRUMENTPLATFORMS_EXPORT SInstrumentIntrinsics
    {
        unsigned int m_nResolutionH;
        unsigned int m_nResolutionV;

        double m_dFieldOfViewH;
        double m_dFieldOfViewV;
        double m_dPrinciplePointH;
        double m_dPrinciplePointV;
        double m_dFocalLengthInPxH;
        double m_dFocalLengthInPxV;
    };

    /** SInstrumentExtrinsics: instrument extrinsic parameters; all in ground reference frame.
      **/
    struct JR_PRO3D_EXTENSIONS_INSTRUMENTPLATFORMS_EXPORT SInstrumentExtrinsics
    {
        const char* m_pcReferenceFrame;
        SPoint3D m_oPosition;
        SVector3D m_oLookAt;
        SVector3D m_oUp;
        SBoundingBox m_oBoundingBox;
    };


    /** SInstrument: instrument inner and outer orientation plus degrees of freedom
      * (defined by axes).
      **/
    struct JR_PRO3D_EXTENSIONS_INSTRUMENTPLATFORMS_EXPORT SInstrument
    {
        const char* m_pcInstrumentName;
        unsigned int m_nNrOfCalibratedFocalLengths;
        double* m_pdCalibratedFocalLengths;
        double m_dCurrentFocalLengthInMm;
        SInstrumentIntrinsics m_oCurrentInstrumentIntrinsics;
        SInstrumentExtrinsics m_oInstrumentExtrinsics;

        unsigned int m_nNrOfInstrumentAxes;
        SAxis** m_poInstrumentAxes; /* list of references to SPlatform member variable */
    };


} // extern "C"

#endif // JR_PRO3D_EXTENSIONS_INSTRUMENTPLATFORMS_HPP
