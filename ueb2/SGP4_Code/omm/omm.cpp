#define _USE_MATH_DEFINES
#include "omm.h"
#include "../helper/mathhelper.h"
#include <iostream>
#include <iomanip>
#include <vector>

/* -------------------------- Constructor --------------------- */
int OMM::valide = 0;
int OMM::notValid = 0;

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
}

/* --------------------------- public functions ------------------------ */

/**
 * @brief Prints the OMM variables to console
 *
 * Prints variables if OMM is valid, an 'invalid' notifier otherwise.
 */
void OMM::print() {
    if (this->valid) {
        cout << "Valid OMMs:" << "\t" << valide << endl;
        cout << "Invalid OMMs:" << "\t" << notValid << endl;
        cout << "Name:" << "\t\t" << satName << "\n";
        cout << "SatNumber:" << "\t" << satID << "\n";
        cout << "Int. Designator:" << "" << intDes << "\n";
        cout << "Year:" << "\t\t" << year << "\n";
        cout << "DayFrac:" << "\t" << setprecision(12) << dayFrac << "\n";
        cout << "bStar:" << "\t\t" << setprecision(12) << bStar << endl;
        cout << "Inclination:" << "\t" << setprecision(12) << inc << " [rad]" << endl;
        cout << "RAAN:" << "\t\t" << setprecision(12) << raan << " [rad]" << endl;
        cout << "Eccentricity:" << "\t" << setprecision(12) << ecc << endl;
        cout << "ArgOfPerigee:" << "\t" << setprecision(12) << argPer << " [rad]" << endl;
        cout << "MeanAnomaly:" << "\t" << setprecision(12) << meanAn << " [rad]" << endl;
        cout << "MeanMotion:" << "\t" << setprecision(10) << meanMot << " [rad/min]" << endl;
        cout << "SemiMajorAxis:" << "\t" << setprecision(12) << calcSemiMajorAxis() << " [km]" << endl;
        cout << "TrueAnomaly:" << "\t" << setprecision(12) << calcTrueAnomaly() << " [rad]" << endl;
    } else {
        cout << "Invalid OMM" << endl;
    }
}

// Aufgabe 1.2
/**
 * @brief Calculates and returns the semi-major axis
 *
 *
 * @return semi-major axis
 */
double OMM::calcSemiMajorAxis() {
    double T = 2 * M_PI / meanMot * 60; //Umlaufzeit berechnen
    double SemiMajor = pow(((G * M * T * T) / (4 * M_PI * M_PI)), 1.0 / 3) / 1000;
    return SemiMajor;
}

// Aufgabe 1.2
/**
 * @brief Calculates and returns the true anomaly
 *
 *
 * @return true anomaly
 */
double OMM::calcTrueAnomaly() {
    double meanAn_deg = rad2deg(meanAn);
    double eccAn = meanAn_deg;
    double eccAn_old;
    double limit = 0.00001;
    while (abs(eccAn_old - eccAn) > limit) {
        eccAn_old = eccAn;
        eccAn = meanAn_deg + 180 / M_PI * ecc * sin(deg2rad(eccAn_old));
    }
    double trueAn = 2.0 * atan2(sqrt(1.0 + ecc) * sin(deg2rad(eccAn) / 2.0),
                                sqrt(1.0 - ecc) * cos(deg2rad(eccAn) / 2.0));
    return trueAn;
}

double OMM::calcOrbitPeriod() {
    double period = 2 * M_PI / meanMot; // [min]
    period *= 60; // [s]
    return period;
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
    for (const string &singleLine: block) {
        // 1. Validierung direkt in der Methode
        // Ein valider KVN-Datensatz MUSS ein Gleichheitszeichen enthalten
        size_t sep = singleLine.find('=');
        if (sep == string::npos) {
            notValid++;
            return false;
        }

        // 2. Extraktion von Key und Value
        string key = singleLine.substr(0, sep);
        string val = singleLine.substr(sep + 1);

        // Trimmen von Leerzeichen
        key.erase(key.find_last_not_of(" ") + 1);
        key.erase(0, key.find_first_not_of(" "));
        val.erase(val.find_last_not_of(" ") + 1);
        val.erase(0, val.find_first_not_of(" "));

        if (val.empty() && (key == "NORAD_CAT_ID" || key == "EPOCH" ||
                            key == "INCLINATION" || key == "MEAN_MOTION" ||
                            key == "ECCENTRICITY")) {
            cout << "Missing value for key: " << key << endl;
            notValid++;
            return false;
        }
        // 3. Zuweisung zu den Variablen aus deinem Listing
        try {
            if (key == "OBJECT_NAME") {
                this->satName = val;
                //strncpy(this->satName, val.c_str(), sizeof(this->satName) - 1);
            } else if (key == "NORAD_CAT_ID") {
                this->satID = stoi(val);
            } else if (key == "OBJECT_ID") {
                this->intDes = val;
                //strncpy(this->intDes, val.c_str(), sizeof(this->intDes) - 1);
            } else if (key == "EPOCH") {
                int y, month, day, hr, min;
                double sec;

                // Parsen des ISO-Strings
                if (sscanf(val.c_str(), "%d-%d-%dT%d:%d:%lf", &y, &month, &day, &hr, &min, &sec) == 6) {
                    this->year = y;

                    // Berechnung des Day-of-Year (DOY)
                    struct tm timeinfo = {0};
                    timeinfo.tm_year = y - 1900;
                    timeinfo.tm_mon = month - 1;
                    timeinfo.tm_mday = day;
                    mktime(&timeinfo);
                    int doy = timeinfo.tm_yday + 1; // tm_yday startet bei 0

                    // Berechnung des Day Fraction (Zeitanteil des Tages)
                    double fraction = (hr / 24.0) + (min / 1440.0) + (sec / 86400.0);
                    this->dayFrac = doy + fraction;
                } else {
                    this->valid = false;
                }
            } else if (key == "BSTAR") {
                this->bStar = stod(val);
            } else if (key == "INCLINATION") {
                // Umrechnung Grad -> Radiant
                this->inc = stod(val) * (M_PI / 180.0);
            } else if (key == "RA_OF_ASC_NODE") {
                this->raan = stod(val) * (M_PI / 180.0);
            } else if (key == "ECCENTRICITY") {
                this->ecc = stod(val);
            } else if (key == "ARG_OF_PERICENTER") {
                this->argPer = stod(val) * (M_PI / 180.0);
            } else if (key == "MEAN_ANOMALY") {
                this->meanAn = stod(val) * (M_PI / 180.0);
            } else if (key == "MEAN_MOTION") {
                // Umrechnung rev/day -> rad/min
                // (Umdrehungen * 2 * PI) / 1440 Minuten pro Tag
                this->meanMot = (stod(val) * 2.0 * M_PI) / 1440.0;
            }
        } catch (...) {
            // Falls stod oder stoi fehlschlägt (z.B. kein numerischer Wert)
            notValid++;
            return false;
        }
    }
    valide++;
    return true;
}
