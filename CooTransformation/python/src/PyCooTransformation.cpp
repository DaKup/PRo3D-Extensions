#include <PyCooTransformation/CooTransformationPy.hpp>

BOOST_PYTHON_MODULE(PyCooTransformation)
{
    bp::def("GetDllVersion", GetDllVersion);
    bp::def("Init", Init);
    bp::def("DeInit", DeInit);
    bp::def("Xyz2LatLonRad", PyXyz2LatLonRad);
    bp::def("Xyz2LatLonAlt", PyXyz2LatLonAlt);
    bp::def("LatLonAlt2Xyz", PyLatLonAlt2Xyz);
}
