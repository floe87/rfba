#ifndef COORDINATES_H_
#define COORDINATES_H_

#define RADIUS_EARTH 6378.137 // in [km]

struct GeocentricCoordinate {
    double latitude, longitude, height;

    void print();
};

struct GeodeticCoordinate {
    double latitude, longitude, height;

    GeodeticCoordinate(double latitude = 0.0, double longitude = 0.0, double height = 0.0);

    void print();
};

struct ECICoordinate {
    float x = 0, y = 0, z = 0;

    void print();
};

struct SEZCoordinate {
    double rS, rE, rZ;

    void print();
};


/**
 *  @brief Converts the Geodetic coordinate to the ECI system
 */
ECICoordinate convertGeodeticToECI(const GeodeticCoordinate &geodCoord, double jd);

/**
 * @brief Transfroms the observation vector OBS -> Sat in the south east zenith system
 */
SEZCoordinate transformECIToSEZ(const ECICoordinate &rObsSat, const GeodeticCoordinate &obs, double jd);

/**
 * @brief Computes the slant range towards the satellite
 */
double computeSlantRange(const SEZCoordinate &sezCoord);

/**
 * @brief Computes the azimuth of the satellite
 */
double computeAzimuth(const SEZCoordinate &sezCoord);

/**
 *
 * @brief Computes the elevation of the satellite
 */
double computeElevation(const SEZCoordinate &sezCoord);

#endif /* COORDINATES_H_ */
