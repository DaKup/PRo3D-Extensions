#include <CooTransformation/CooTransformation.hpp>

#include <boost/python.hpp>

namespace bp = boost::python;

bp::tuple PyXyz2LatLonRad(double dX, double dY, double dZ)
{
    double dLat, dLon, dRad;
    int nRet = Xyz2LatLonRad(dX, dY, dZ, &dLat, &dLon, &dRad);
    if (nRet != 0)
    {
        throw nRet;
    }
    return bp::make_tuple(dLat, dLon, dRad);
}

bp::tuple PyXyz2LatLonAlt(const char* pcPlanet, double dX, double dY, double dZ)
{
    double dLat, dLon, dAlt;
    int nRet = Xyz2LatLonAlt(pcPlanet, dX, dY, dZ, &dLat, &dLon, &dAlt);
    if (nRet != 0)
    {
        throw nRet;
    }
    return bp::make_tuple(dLat, dLon, dAlt);
}

bp::tuple PyLatLonAlt2Xyz(const char* pcPlanet, double dLat, double dLon, double dAlt)
{
    double dX, dY, dZ;
    int nRet = LatLonAlt2Xyz(pcPlanet, dLat, dLon, dAlt, &dX, &dY, &dZ);
    if (nRet != 0)
    {
        throw nRet;
    }
    return bp::make_tuple(dX, dY, dZ);
}
