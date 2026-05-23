#ifndef HELPER_TIMEDATE_H_
#define HELPER_TIMEDATE_H_

#define _USE_MATH_DEFINES

/**
 * @brief Computes the Julian Date for an OMM
 */
double computeJD(int year, double dayFraction);

/**
 * @brief Computes Greenwich Mean Sideral time at given JD
 */
double computeGMST(double jd);


#endif /* HELPER_TIMEDATER_H_ */
