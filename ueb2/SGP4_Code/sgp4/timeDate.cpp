#include "timeDate.h"
#include "../helper/mathhelper.h"

// Aufgabe 2.2
double computeJD(int year, double dayFraction) {
    // source: https://de.wikipedia.org/wiki/Julianisches_Datum
    // calculate Julian Date of January 1st 00:00 UTC
    const int Y = year - 1;
    constexpr int M = 13; // January = 1 + 12
    const double B = 2 - floor(Y / 100.0) + floor(Y / 400.0); // gregorian calendar correction term
    const double jdJanFirst = floor(365.25 * (Y + 4716.0)) + floor(30.6001 * (M + 1)) + 1 + B - 1524.5;

    // adding day fraction
    return jdJanFirst + dayFraction - 1;
}

double computeGMST(double jd) {
    // source: Vorlesungsfolie RFBA_4
    double SECONDS_PER_DAY = 86400;
    double omegaEarth = 7.29211510e-5; // angular velocity of Earth in [rad/s]

    // calculate JD(0h): Julian date at 00:00 UTC of same calendar day
    double jdZeroHour = floor(jd + 0.5) - 0.5; // julian days start 12:00 UTC

    // calculate elapsed seconds since JD(0h)
    double elapsedSecondsSinceJdZeroHour = (jd - jdZeroHour) * SECONDS_PER_DAY;

    // calculate julian centuries for JD(0h) since J2000
    double Tu = (jdZeroHour - 2451545.0) / 36525.0;

    // calculate GMST(0h): Greenwich mean siderial time at 00:00 UTC in sidereal seconds
    double gmstZeroHourInSeconds = 24110.54841 + 8640184.812866 * Tu + 0.093104 * pow(Tu, 2) - 0.0000062 * pow(Tu, 3);

    // convert GMST(0h) from sidereal seconds to radians
    double gmstZeroHourInRad = gmstZeroHourInSeconds * TWO_PI/SECONDS_PER_DAY;

    // calculate GMST(T) at the given point in time
    double gmstT = gmstZeroHourInRad + omegaEarth * elapsedSecondsSinceJdZeroHour;

    // reduce to interval [0;2PI)
    gmstT = fmod(gmstT, TWO_PI);
    if (gmstT < 0.0) {
        gmstT += TWO_PI;
    }
    return gmstT;
}
