#include "coordinates.h"
#include "../helper/mathhelper.h"
#include "timeDate.h"
#include <cmath>
#include <iostream>

// TODO Erg�nzen Sie in dieser Datei convertGeodeticToECI, transformECIToSEZ, computeSlantRange, computeAzimuth und computeElevation

/* -------------------------- Geocentric Coordinate ------------------------------ */

void GeocentricCoordinate::print() {
    std::cout << rad2deg(latitude) << ", " << rad2deg(longitude) << ", "
            << height;
}

/* -------------------------- Geodetic Coordinate -------------------------------- */

GeodeticCoordinate::GeodeticCoordinate(double latitude, double longitude, double height) : latitude(latitude),
    longitude(longitude), height(height) {
}

void GeodeticCoordinate::print() {
    std::cout << rad2deg(latitude) << ", " << rad2deg(longitude) << ", "
            << height;
}

/* -------------------------- ECI Coordinate ------------------------------------- */

void ECICoordinate::print() {
    std::cout << x << ", " << y << ", " << z;
}

/* -------------------------- South East Zenith Coordinate ----------------------- */

void SEZCoordinate::print() {
    std::cout << rS << ", " << rE << ", " << rZ;
}

ECICoordinate convertGeodeticToECI(const GeodeticCoordinate &geodCoord, double jd) {
    ECICoordinate eciCoord;
    // source for WGS-84 reference values: https://de.wikipedia.org/wiki/World_Geodetic_System_1984
    double a = RADIUS_EARTH; // semi Major axis in [km]
    const double f = 1.0 / 298.257223563; // flattening of Earth

    // source for calculation: Vorlesungsfolie RFBA_4, Folie 11
    // and for transforming https://en.wikipedia.org/wiki/Geodetic_coordinates
    // helping variables
    const double C = 1 / sqrt(1 + f * (f - 2) * pow(sin(geodCoord.latitude), 2.0));
    const double S = pow((1 - f), 2.0) * C;
    const double localSiderialTime = computeGMST(jd) + geodCoord.longitude;

    // ECI-Coordinates for object at height 'h' over the surface of WGS84-ellipsoid
    eciCoord.x = static_cast<float>((a * C + geodCoord.height) * cos(geodCoord.latitude) * cos(localSiderialTime));
    eciCoord.y = static_cast<float>((a * C + geodCoord.height) * cos(geodCoord.latitude) * sin(localSiderialTime));
    eciCoord.z = static_cast<float>((a * S + geodCoord.height) * sin(geodCoord.latitude));

    return eciCoord;
}

SEZCoordinate transformECIToSEZ(const ECICoordinate &rObsSat, const GeodeticCoordinate &obs, double jd) {
    // source for calculation: Vorlesungsfolie RFBA_4, Folie 9
    SEZCoordinate sezCoord{};
    const double siderialTimeObs = computeGMST(jd) + obs.longitude;
    const double phiObs = obs.latitude;

    // south-direction
    sezCoord.rS = sin(phiObs) * cos(siderialTimeObs) * rObsSat.x
                  + sin(phiObs) * sin(siderialTimeObs) * rObsSat.y
                  - cos(phiObs) * rObsSat.z;

    // east-direction
    sezCoord.rE = -sin(siderialTimeObs) * rObsSat.x + cos(siderialTimeObs) * rObsSat.y;

    // zenith-direction
    sezCoord.rZ = cos(phiObs) * cos(siderialTimeObs) * rObsSat.x
                  + cos(phiObs) * sin(siderialTimeObs) * rObsSat.y
                  + sin(phiObs) * rObsSat.z;

    return sezCoord;
}

double computeSlantRange(const SEZCoordinate &sezCoord) {
    return sqrt(sezCoord.rS * sezCoord.rS
                + sezCoord.rE * sezCoord.rE
                + sezCoord.rZ * sezCoord.rZ);
}

double computeElevation(const SEZCoordinate &sezCoord) {
    const double elevation = asin(sezCoord.rZ / computeSlantRange(sezCoord));
    // return value of asin is already placed in intervall [-PI/2,PI/2]
    return elevation;
}

double computeAzimuth(const SEZCoordinate &sezCoord) {
    double azimuth = atan2(sezCoord.rE, -sezCoord.rS);
    // reduce to intervall [0,2*PI)
    if (azimuth < 0.0) {
        azimuth += TWO_PI;
    }
    return azimuth;
}
