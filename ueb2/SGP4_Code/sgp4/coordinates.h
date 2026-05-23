#ifndef COORDINATES_H_
#define COORDINATES_H_


struct GeocentricCoordinate {
	// TODO Ergänzen Sie Variablen und Funktionalitäten
};

struct GeodeticCoordinate {
	// TODO Ergänzen Sie Variablen und Funktionalitäten
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
