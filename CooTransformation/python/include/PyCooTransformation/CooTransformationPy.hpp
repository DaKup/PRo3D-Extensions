#include <CooTransformation/CooTransformation.hpp>

#include <pybind11/pybind11.h>

namespace py = pybind11;

py::tuple PyXyz2LatLonRad(double dX, double dY, double dZ)
{
    double dLat, dLon, dRad;
    int nRet = Xyz2LatLonRad(dX, dY, dZ, &dLat, &dLon, &dRad);
    if (nRet != 0)
    {
        throw nRet;
    }
    return py::make_tuple(dLat, dLon, dRad);
}

py::tuple PyXyz2LatLonAlt(const char* pcPlanet, double dX, double dY, double dZ)
{
    double dLat, dLon, dAlt;
    int nRet = Xyz2LatLonAlt(pcPlanet, dX, dY, dZ, &dLat, &dLon, &dAlt);
    if (nRet != 0)
    {
        throw nRet;
    }
    return py::make_tuple(dLat, dLon, dAlt);
}

py::tuple PyLatLonAlt2Xyz(const char* pcPlanet, double dLat, double dLon, double dAlt)
{
    double dX, dY, dZ;
    int nRet = LatLonAlt2Xyz(pcPlanet, dLat, dLon, dAlt, &dX, &dY, &dZ);
    if (nRet != 0)
    {
        throw nRet;
    }
    return py::make_tuple(dX, dY, dZ);
}
