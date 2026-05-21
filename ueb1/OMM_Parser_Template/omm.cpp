#define _USE_MATH_DEFINES
#include "omm.h"
#include "mathhelper.h"
#include <iostream>
#include <iomanip>
#include <vector>

/* -------------------------- Constructor --------------------- */
OMM::OMM() : valid(false) {
}

/**
 * @brief Creates a new OMM object from the raw data contained in the file.
 *
 * Checks if the OMM is valid and sets the valid variable accordingly. Then parses 
 * the OMM into the class variables.
 *
 * @param block the raw data of the OMM as a vector of strings
 */
OMM::OMM(vector<string> block) : OMM() {
    valid = populate(block);

    if (this->getIsValid()) {
        // Aufgabe 1.1
        this->inc = deg2rad(this->inc);
        this->raan = deg2rad(this->raan);
        this->argPer = deg2rad(this->argPer);
        this->meanAn = deg2rad(this->meanAn);
        this->meanMot = deg2rad(this->meanMot * 360) / (24 * 60); // conversion from rev/day to rad/min

        // Aufgabe 1.2
        this->SemiMajorAxis = calcSemiMajorAxis();
        this->trueAn = calcTrueAnomaly();
    }
}

/* --------------------------- public functions ------------------------ */

/**
 * @brief Prints the OMM variables to console
 * 
 * Prints variables if OMM is valid, an 'invalid' notifier otherwise.
 */
void OMM::print() {
    if (this->valid) {
        printf("Entry Status:\t\tValid\n"
               "Name of the satellite:\t\t%s\n"
               "Satellite catalog nr.:\t\t%d\n"
               "International designator:\t%s\n"
               "Year of the epoch:\t\t%d\n"
               "Day fraction of the epoch:\t%f\n"
               "Drag term:\t\t\t%f\n"
               "Inclination:\t\t\t%f\t[rad]\n"
               "RAAN:\t\t\t\t%f\t[rad]\n"
               "Eccentricity of the orbit:\t%f\n"
               "Argument of perigee:\t\t%f\t[rad]\n"
               "Mean anomaly:\t\t\t%f\t[rad]\n"
               "Mean motion:\t\t\t%f\t[rad/min]\n"
               "Semi Major Axis: \t\t%f\t[km]\n"
               "True Anomaly: \t\t\t%f\t[rad]\n",
               this->satName, this->satID, this->intDes, this->year, this->dayFrac, this->bStar,
               this->inc, this->raan, this->ecc, this->argPer, this->meanAn, this->meanMot,
               this->SemiMajorAxis,this->trueAn);
    } else
        printf("Entry Status:\t\tInvalid OMM\n");
}

int OMM::getSatID() {
    return this->satID;
}

bool OMM::getIsValid() {
    return this->valid;
}


// Aufgabe 1.2
/**
 * @brief Calculates and returns the semi-major axis
 * 
 *
 * @return semi-major axis
 */
double OMM::calcSemiMajorAxis() {
    const double GMEarth = 398600.0; // Standard gravitational parameter of earth in km^3/s^2
    const double T = (2.0 * M_PI / this->meanMot) * 60.0; // Orbital period in s
    return pow(GMEarth * pow(T, 2) / (4 * pow(M_PI, 2)), 1.0 / 3.0);
}

// Aufgabe 1.2
/**
 * @brief Calculates and returns the true anomaly
 *
 *
 * @return true anomaly
 */
double OMM::calcTrueAnomaly() {
    // calculate the eccentric anomaly E using the newton iteration
    double eccCurrentValue; // E_n value

    // starting value depending on eccentricity of orbit
    if (this->ecc > 0.8 && this->ecc < 1.0) {
        eccCurrentValue = M_PI;
    } else eccCurrentValue = this->meanAn;

    //calculate eccentric Anomaly
    bool convergedResult = false;
    double diff = INFINITY; // safe starting value for difference between E_n and E_n+1.
    for (int i = 0; i < 20; ++i) {
        double fE = eccCurrentValue - this->ecc * sin(eccCurrentValue) - this->meanAn;
        double dfE = 1 - this->ecc * cos(eccCurrentValue);
        // formula for newton iteration
        double eccNextValue = eccCurrentValue - fE / dfE; // E_n+1 value
        diff = fabs(eccNextValue - eccCurrentValue);
        if (double epsilon = 1e-6; diff < epsilon) {
            eccCurrentValue = eccNextValue;
            convergedResult = true;
            break;
        }
        eccCurrentValue = eccNextValue;
    }
    if (!convergedResult) {
        printf("Newton's method to calculate eccentric Anomaly not finished - Remaining Difference: %f for ID:%d\n",
               diff, this->satID);
    }
    // calculate true anomaly with result of E
    double trueAnomaly = 2 * atan2(
                             sin(eccCurrentValue / 2) * sqrt(1 + this->ecc),
                             cos(eccCurrentValue / 2) * sqrt(1 - this->ecc));

    // normalize result of 2 * atan2 if placed between 0 and -2 * PI.
    if (trueAnomaly < 0.0) {
        trueAnomaly += 2 * M_PI;
    }
    return trueAnomaly;
}

/* ----------------------- private functions ---------------------------- */

/**
 * @brief Populates the class variables from a OMM.
 *
 * First checks if the OMM is valid. Then parses the OMM into the
 * class variables.
 *
 * @param block the raw data of the OMM as a vector of strings 
 * 
 *
 * @return true if the OMM was parsed, false otherwise
 */
bool OMM::populate(vector<string> block) {
    int validPos = 0;
    // loop through all lines of OMM block
    for (const string &SingleEntry: block) {
        // split line into "key = value"
        constexpr char Divider = '=';
        const size_t DividerPos = SingleEntry.find(Divider);
        string key = SingleEntry.substr(0, DividerPos);
        string value = SingleEntry.substr(DividerPos + 1);

        // remove trailing spaces from key
        key.erase(key.find_last_not_of(' ') + 1);
        // remove leading spaces from value
        value.erase(0, value.find_first_not_of(' '));

        // check for key and if value is physically valid
        if (key == "OBJECT_NAME") {
            if (value.length() <= 25) {
                strcpy(this->satName, value.c_str());
                validPos++;
            } else printf("%s: Invalid OBJECT_NAME\n", value.c_str());
        } else if (key == "NORAD_CAT_ID") {
            if (value.length() == 5 && stoi(value) > 0) {
                this->satID = stoi(value);
                validPos++;
            } else printf("%s: Invalid NORAD_CAT_ID\n", value.c_str());
        } else if (key == "OBJECT_ID") {
            size_t divPos = value.find('-');
            if (divPos != string::npos) {
                value.erase(divPos, 1); //delete only one character
            }
            if (value.length() <= 9) {
                strcpy(this->intDes, value.c_str());
                validPos++;
            } else printf("%s: Invalid OBJECT_ID\n", value.c_str());
        } else if (key == "EPOCH") {
            if (value.length() == 26) {
                int year = stoi(value.substr(0, 4));
                int month = stoi(value.substr(5, 2));
                int day = stoi(value.substr(8, 2));
                int hour = stoi(value.substr(11, 2));
                int minute = stoi(value.substr(14, 2));
                double second = stod(value.substr(17));
                if ((year > 1950 && year < 2027) && (month >= 1 && month <= 12)
                    && (day >= 1 && day <= 31) && (hour >= 0 && hour <= 23)
                    && (minute >= 0 && minute <= 59) && (second >= 0.0 && second < 60.0)) {
                    if (year != 2026) printf("year is not 2026 - dayFrac might be wrong");
                    this->year = year;
                    // assumption for following conversion: year is no leap year (2026)
                    int daysUntilMonth[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
                    this->dayFrac = daysUntilMonth[month - 1] + day + hour / 24.0 + minute / 1440.0 + second / 86400.0;
                    validPos++;
                }
            } else printf("%s: Invalid EPOCH\n", value.c_str());
        } else if (key == "BSTAR") {
            double dragCoeff = stod(value);
            if (dragCoeff >= 0.0 && dragCoeff < 1.0) {
                this->bStar = stod(value);
                validPos++;
            } else printf("%s: Invalid BSTAR\n", value.c_str());
        } else if (key == "INCLINATION") {
            double degInclination = stod(value);
            // prograde and retrograde orbits
            if (degInclination >= 0.0 && degInclination <= 180.0) {
                this->inc = degInclination;
                validPos++;
            } else printf("%s: Invalid INCLINATION\n", value.c_str());
        } else if (key == "RA_OF_ASC_NODE") {
            double degRaan = stod(value);
            if (degRaan >= 0.0 && degRaan < 360.0) {
                this->raan = degRaan;
                validPos++;
            } else printf("%s: Invalid RA_OF_ASC_NODE\n", value.c_str());
        } else if (key == "ECCENTRICITY") {
            // allowing only circular to elliptical orbits
            double eccentricity = stod(value);
            if (eccentricity >= 0.0 && eccentricity < 1.0) {
                this->ecc = eccentricity;
                validPos++;
            } else printf("%s: Invalid ECCENTRICITY\n", value.c_str());
        } else if (key == "ARG_OF_PERICENTER") {
            double degArgPer = stod(value);
            if (degArgPer >= 0.0 && degArgPer < 360.0) {
                this->argPer = degArgPer;
                validPos++;
            } else printf("%s: Invalid ARG_OF_PERICENTER\n", value.c_str());
        } else if (key == "MEAN_ANOMALY") {
            double degMeanAn = stod(value);
            if (degMeanAn >= 0.0 && degMeanAn < 360.0) {
                this->meanAn = degMeanAn;
                validPos++;
            } else printf("%s: Invalid MEAN_ANOMALY\n", value.c_str());
        } else if (key == "MEAN_MOTION") {
            // revs per day
            double meanMotInRevPerDay = stod(value);
            if (meanMotInRevPerDay >= 0.0 && meanMotInRevPerDay <= 20.0) {
                this->meanMot = meanMotInRevPerDay;
                validPos++;
            } else printf("%s: Invalid MEAN_MOTION\n", value.c_str());
        }
    }
    // only true for physically valid values in all variables
    if (validPos == 11) {
        return true;
    }
    return false;
}
