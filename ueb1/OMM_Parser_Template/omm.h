/*
 * omm.h
 *
 *  Created on: 17.07.2019
 *      Author: MaurerAndreas
 *  Edited by: Dominik Regele 04.05.2026
 */

#ifndef OMM_H_
#define OMM_H_

#include <stdint.h>
#include <string>
#include <cstring>
#include <cstdio>
#include <stdlib.h>
#include <ctype.h>
#include <cmath>
#include "mathhelper.h"
#include <vector>

using namespace std;
class OMM
{
private:
	char satName [25] = {'\0'};				// name of the satellite
	int satID;								// satellite catalog nr
	char intDes [9] = {'\0'};				// international designator
	int year;								// year of epoch
	double dayFrac;							// day fraction of the epoch
	double bStar;							// drag term

	double inc;								// inclination [rad]
	double raan;							// right ascension of ascending node [rad]
	double ecc;								// eccentricity of the orbit
	double argPer;							// argument of perigee [rad]
	double meanAn;							// mean anomaly [rad]
	double meanMot;							// mean motion [rad/min]

	bool valid = false;						// indicates if the OMM is valid

	double SemiMajorAxis;					// semi-major-axis of orbit [km]
	double trueAn;							// true anomaly [rad]


	bool populate(vector<string> block);

public:
	OMM();
	OMM(vector<string> block);

    /**
     * @brief Prints OMM variables to console if OMM is valid. Otherwise prints "INVALID OMM".
     */
    void print();

    /**
     * @brief Calculates and returns the semi-major axis
     * @return semi-major axis
     */
    double calcSemiMajorAxis();

    /**
     * @brief Calculates and returns the true anomaly
     * @return true anomaly
     */
    double calcTrueAnomaly();

	// getter methods
	int getSatID();
	bool getIsValid();
};

#endif /* OMM_H_ */
