#ifndef COORDINATES_H_
#define COORDINATES_H_

#define RADIUS_EARTH 6378.137 // in [km]

struct GeocentricCoordinate {
    double latitude; // geocentric latitude [rad]
    double longitude; // longitude [rad]
    double height; // height above spherical Earth [km]
};

struct GeodeticCoordinate {
    double latitude; // geodetic latitude [rad]
    double longitude; // longitude [rad]
    double height; // height above WGS-84 ellipsoid [km]
};

/**
 * @brief Simple 3D Cartesian coordinate in ECI
 */
struct ECICoordinate {
    double x;
    double y;
    double z;
};


/**
 * @brief Converts the ECI coordinate to the geodetic system
 */
GeodeticCoordinate convertECItoGeodetic(const ECICoordinate &eciCoord, double jd);

/**
 * @brief Converts the ECI coordinate to the geocentric system
 */
GeocentricCoordinate convertECItoGeocentric(const ECICoordinate &eciCoord, double jd);

#endif /* COORDINATES_H_ */
