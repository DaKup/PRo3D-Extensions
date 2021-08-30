#include <PyCooTransformation/CooTransformationPy.hpp>

PYBIND11_MODULE(PyCooTransformation, m)
{
    m.def("GetDllVersion", GetDllVersion);
    m.def("Init", Init);
    m.def("DeInit", DeInit);
    m.def("Xyz2LatLonRad", PyXyz2LatLonRad);
    m.def("Xyz2LatLonAlt", PyXyz2LatLonAlt);
    m.def("LatLonAlt2Xyz", PyLatLonAlt2Xyz);
}

