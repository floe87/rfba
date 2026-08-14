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
#include "../helper/mathhelper.h"
#include <vector>

using namespace std;
class OMM
{
private:
    string satName;
    int satID;
    string intDes;
    int year;
    double dayFrac;
    double bStar;

    double inc;
    double raan;
    double ecc;
    double argPer;
    double meanAn;
    double meanMot;


    double nDot;
    double nDotDot;
    int revsAtEpoch;

	bool valid = false;

    static int valide;
    static int notValid;
    static constexpr double G = 6.67430e-11;
    static constexpr double M = 5.972e24;
    
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

	/**
	* @brief Calculates and returns the orbit period
	* @return orbit period [s]
	*/
	double calcOrbitPeriod();

    inline string  getSatelliteName() const
	{
		return satName;
	}

    inline int32_t getSatelliteNr() const
	{
		return satID;
	}

    inline string  getIntDesignator() const {
		return intDes;
	}

	inline int32_t getYear() const
	{
		return year;
	}

	inline double getDayFraction() const
	{
		return dayFrac;
	}

	inline double getnDot() const
	{
		return nDot;
	}

	inline double getnDotDot() const
	{
		return nDotDot;
	}

	inline double getBstar() const
	{
		return bStar;
	}

	inline double getInclination() const
	{
		return inc;
	}

	inline double getRaan() const
	{
		return raan;
	}

	inline double getEccentricity() const
	{
		return ecc;
	}

	inline double getArgumentOfPerigee() const
	{
		return argPer;
	}

	inline double getMeanAnomaly() const
	{
		return meanAn;
	}

	inline double getMeanMotion() const
	{
		return meanMot;
	}

	inline int32_t getRevolutionAtEpoch() const
	{
		return revsAtEpoch;
	}
};

#endif /* OMM_H_ */
