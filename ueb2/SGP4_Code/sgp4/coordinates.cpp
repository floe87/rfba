#include "coordinates.h"
#include "timeDate.h"
#include "../helper/mathhelper.h"
#include <iostream>

using namespace std;

// Aufgabe 2.2
GeocentricCoordinate convertECItoGeocentric(const ECICoordinate &eciCoord, double jd) {
    GeocentricCoordinate geocentricCoord{};

    // source for Calculation: Vorlesungsfolie RFBA_4
    const double R = sqrt(pow(eciCoord.x, 2.0) + pow(eciCoord.y, 2.0));

    // latitude calculation:
    geocentricCoord.latitude = atan2(eciCoord.z, R);

    // longitude calculation:
    geocentricCoord.longitude = atan2(eciCoord.y, eciCoord.x) - computeGMST(jd);
    // reduce to intervall (-PI; PI]
    while (geocentricCoord.longitude <= -M_PI) {
        geocentricCoord.longitude += TWO_PI;
    }
    while (geocentricCoord.longitude > M_PI) {
        geocentricCoord.longitude -= TWO_PI;
    }

    // height calculation:
    geocentricCoord.height = sqrt(pow(eciCoord.x, 2.0) + pow(eciCoord.y, 2.0) + pow(eciCoord.z, 2.0)) - RADIUS_EARTH;

    return geocentricCoord;
}

GeodeticCoordinate convertECItoGeodetic(const ECICoordinate &eciCoord, double jd) {
    GeodeticCoordinate geodeticCoord{};

    // source for WGS-84 reference values: https://de.wikipedia.org/wiki/World_Geodetic_System_1984
    double a = RADIUS_EARTH; // semi Major axis in [km]
    double flatteningEarth = 1.0 / 298.257223563;

    // source for calculation: Vorlesungsfolie RFBA_4
    double R = sqrt(pow(eciCoord.x, 2.0) + pow(eciCoord.y, 2.0));
    double eSquared = 2.0 * flatteningEarth - pow(flatteningEarth, 2.0);

    // latitude calculation:
    // choose geocentric latitude as starting value for iteration
    double latCurrentValue = atan2(eciCoord.z, R);
    // iteration steps for geodetic latitude
    bool convergedResult = false;
    for (int i = 0; i < 20; ++i) {
        const double C = 1.0 / sqrt(1 - eSquared * pow(sin(latCurrentValue), 2.0));
        const double latNextValue = atan2(eciCoord.z + a * C * eSquared * sin(latCurrentValue), R);
        const double diff = fabs(latNextValue - latCurrentValue);
        if (constexpr double epsilon = 1e-6; diff < epsilon) {
            latCurrentValue = latNextValue;
            convergedResult = true;
            break;
        }
        latCurrentValue = latNextValue;
    }
    if (!convergedResult) {
        cout << "Value für geodetic latitude did not converge" << endl;
    }
    geodeticCoord.latitude = latCurrentValue;

    // longitude calculation:
    geodeticCoord.longitude = atan2(eciCoord.y, eciCoord.x) - computeGMST(jd);
    // reduce to intervall (-PI; PI]
    while (geodeticCoord.longitude <= -M_PI) {
        geodeticCoord.longitude += TWO_PI;
    }
    while (geodeticCoord.longitude > M_PI) {
        geodeticCoord.longitude -= TWO_PI;
    }

    // height calculation:
    const double C = 1.0 / sqrt(1.0 - eSquared * pow(sin(geodeticCoord.latitude), 2.0));
    geodeticCoord.height = R / cos(geodeticCoord.latitude) - a * C;

    return geodeticCoord;
}
