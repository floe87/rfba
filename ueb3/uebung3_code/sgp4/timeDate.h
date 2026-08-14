#ifndef HELPER_TIMEDATE_H_
#define HELPER_TIMEDATE_H_

#define _USE_MATH_DEFINES

#include <stdint.h>

/**
 * @brief Computes the Julian Date for a TLE
 *
 */
double computeJD(int year, double dayFraction);

/**
 * @brief Computes Greenwich Mean Sideral time at given JD
 */
double computeGMST(double jd);

/**
 * @brief Implements the Gregorian Calendar
 */
struct GregorianCalendar {
    int16_t year; // year
    int8_t month; // month [1, 12]
    int8_t day; // day
    int8_t hour; // hour [0, 24)
    int8_t min; // minute [0, 60)
    int8_t sec; // second [0, 60]

    // TODO Erg�nzen Sie eventuell ben�tigte Funktionalit�ten
    void print();
};

/**
 * @brief Converts the Gregorian Calendar to its Julian Date
 *
 */
double computeJDFromGregCal(const GregorianCalendar &gregCal);

/**
 * @brief Computes the Gregorian Calendar from the Julian Date
 */
GregorianCalendar computeGregCalFromJD(double jd);

#endif /* HELPER_TIMEDATER_H_ */
