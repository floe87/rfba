#include "timeDate.h"
#include "../helper/mathhelper.h"
#include <stdint.h>
#include <iostream>
#include <iomanip>

#define _USE_MATH_DEFINES

// TODO Erg�nzen Sie hier computeJDFromGregCal und computeGregCalFromJD sowie Funktionalit�ten des GregorianCalendar structs
double computeJDFromGregCal(const GregorianCalendar &gregCal) {
    // source : https://de.wikipedia.org/wiki/Julianisches_Datum
    int Y = 0;
    int M = 0;
    if (gregCal.month > 2) {
        Y = gregCal.year;
        M = gregCal.month;
    } else {
        Y = gregCal.year - 1;
        M = gregCal.month + 12;
    }
    // calculate Day Fraction
    const double D = gregCal.day + gregCal.hour / 24.0 + gregCal.min / 1440.0 + gregCal.sec / 86400.0;

    // gregorian calendar correction term
    const double B = 2 - floor(Y / 100.0) + floor(Y / 400.0);

    // calculate julian date
    const double jd = floor(365.25 * (Y + 4716.0)) + floor(30.6001 * (M + 1)) + D + B - 1524.5;

    return jd;
}

GregorianCalendar computeGregCalFromJD(double jd) {
    // source : https://de.wikipedia.org/wiki/Julianisches_Datum
    GregorianCalendar gregCal{};

    const double Z = floor(jd + 0.5);
    const double F = jd + 0.5 - Z;

    const double alpha = floor((Z - 1867216.25) / 36524.25);
    const double A = Z + 1.0 + alpha - floor(alpha / 4.0);

    const double B = A + 1524.0;
    const double C = floor((B - 122.1) / 365.25);
    const double D = floor(365.25 * C);
    const double E = floor((B - D) / 30.6001);

    const double dayFrac = B - D - floor(30.6001 * E) + F;
    gregCal.day = floor(dayFrac);
    const int elapsedSecondsOfDay = static_cast<int>(round((dayFrac - gregCal.day) * 86400.0));
    gregCal.sec = static_cast<int8_t>(elapsedSecondsOfDay % 60);
    gregCal.min = static_cast<int8_t>((elapsedSecondsOfDay % 3600) / 60);
    gregCal.hour = static_cast<int8_t>(elapsedSecondsOfDay / 3600);

    if (E <= 13.0) {
        gregCal.month = static_cast<int8_t>(E - 1.0);
        gregCal.year = static_cast<int16_t>(C - 4716.0);
    } else {
        gregCal.month = static_cast<int8_t>(E - 13.0);
        gregCal.year = static_cast<int16_t>(C - 4715.0);
    }
    return gregCal;
}

void GregorianCalendar::print() {
    // format dd.MM.yyyy hh:mm:ss
    std::cout << std::setfill('0')
            << std::setw(2) << static_cast<int>(day) << "."
            << std::setw(2) << static_cast<int>(month) << "."
            << std::setw(4) << static_cast<int>(year) << " "
            << std::setw(2) << static_cast<int>(hour) << ":"
            << std::setw(2) << static_cast<int>(min) << ":"
            << std::setw(2) << static_cast<int>(sec) << std::endl;
}

/**
 * @brief Computes the Julian Date for a TLE
 *
 */
double computeJD(int year, double dayFraction) {
    // JD at 1.1.1900 == 2415020.5, add years and day fraction accordingly, consider leap years
    return (2415020.5 + (year - 1900) * 365 + static_cast<int32_t>((year - 1900 - 1) / 4)) + dayFraction - 1;
}

/**
 * @brief Computes Greenwich Mean Sideral time at given JD
 *
 * @note Calculation according to RFBA_4.pdf, slide 5
 */
double computeGMST(double jd) {
    static double we = 7.29211510e-5; ///< rotation rate of Earth rad/s

    // convert JD to 0h, consider 12h offset of JD
    uint32_t temp = (uint32_t) jd;
    double JD0h; ///< jd at 0 UTC
    if ((temp + 0.5) > jd)
        JD0h = temp - 0.5;
    else
        JD0h = temp + 0.5;

    //compute Julian century
    double Tu = (JD0h - 2451545.0) / 36525.0;

    //gmst in rad
    double gmst0h = (24110.54841 + ((-0.0000062 * Tu + 0.093104) * Tu + 8640184.812866) * Tu) * M_PI / 43200.0;
    //limit between -2 PI and 2Pi
    gmst0h = fmod(gmst0h, TWO_PI);

    //compute time since 0h UTC
    double T = (jd - JD0h) * 24 * 3600;
    double gmst = gmst0h + we * T;

    //limit between 0 and 2Pi
    gmst = fmod(gmst, TWO_PI);
    if (gmst < 0)
        gmst += TWO_PI;

    return gmst;
}
