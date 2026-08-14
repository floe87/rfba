#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <map>
#include "omm/omm.h"
#include "omm/ommreader.h"
#include "sgp4/timeDate.h"
#include "sgp4/coordinates.h"
#include "sgp4/SGP4Propagator.h"


int main(int argc, char *argv[]) {
    // File with OMMS
    string fileName = "sonate2.txt";

    // Get Map from txt file
    std::map<int, OMM> satMap = readOmmFile(fileName.c_str());

    // READ OMM from SONATE-2
    OMM omm = satMap.at(59112);

    // SGP4 Propagator
    SGP4Propagator sgp4Propagator;
    sgp4Propagator.setOmm(omm);

    // Aufgabe: Ergänzen Sie hier Ihren Code
    cout << fixed << setprecision(10);
    fstream myFile;

    // Aufgabe 3.1
    GregorianCalendar ObsPeriodStartDate{2025, 6, 20, 15, 0, 0};
    double jdObsPeriodStart = computeJDFromGregCal(ObsPeriodStartDate);
    double jdEpochOfOMM = computeJD(omm.getYear(), omm.getDayFraction());
    // calculate time difference in seconds: positiv for date after epoch; negativ for date before epoch
    double secondsEpochToObsPeriodStart = (jdObsPeriodStart - jdEpochOfOMM) * 86400.0;

    myFile.open("jd-and-eci-data.csv", ios::out);
    if (!myFile.is_open()) {
        cout << "Could not open file" << endl;
        return 1;
    }
    myFile << fixed << setprecision(10);
    myFile << "UTC-Time on 20.06.2025" << ","
            << "Julian Date [days]" << ","
            << "ECI X-Position [km]" << ","
            << "ECI Y-Position [km]" << ","
            << "ECI Z-Position [km]" << endl;
    // 2 hour time period, 30 second intervall
    for (int elapsedSeconds = 0; elapsedSeconds <= 2 * 3600; elapsedSeconds += 30) {
        ECICoordinate satPos, satVel;
        auto targetTime = static_cast<int32_t>(round(elapsedSeconds + secondsEpochToObsPeriodStart));
        sgp4Propagator.calculatePositionAndVelocity(targetTime, satPos, satVel);
        double jdCurrent = jdObsPeriodStart + elapsedSeconds / 86400.0;
        // timestamps start at 15:00 UTC
        int hour = 15 + elapsedSeconds / 3600;
        int minute = (elapsedSeconds % 3600) / 60;
        int second = elapsedSeconds % 60;
        myFile << setw(2) << setfill('0') << hour << ":"
                << setw(2) << setfill('0') << minute << ":"
                << setw(2) << setfill('0') << second << ","
                << jdCurrent << ","
                << satPos.x << ","
                << satPos.y << ","
                << satPos.z << endl;
    }
    myFile.close();

    // Aufgabe 3.2
    // Ground Station
    GeodeticCoordinate geodeticGroundStationPos{deg2rad(49.78), deg2rad(9.97), 0.275};

    myFile.open("jd-and-tracking-data.csv", ios::out);
    if (!myFile.is_open()) {
        cout << "Could not open file" << endl;
        return 1;
    }
    myFile << fixed << setprecision(10);
    myFile << "Julian Date [days]" << ","
            << "Satellite ECI X-Position [km]" << ","
            << "Satellite ECI Y-Position [km]" << ","
            << "Satellite ECI Z-Position [km]" << ","
            << "Ground Station ECI X-Position [km]" << ","
            << "Ground Station ECI Y-Position [km]" << ","
            << "Ground Station ECI Z-Position [km]" << ","
            << "SEZ-South Distance [km]" << ","
            << "SEZ-East Distance [km]" << ","
            << "SEZ-Zenith Distance [km]" << ","
            << "Slant Range [km]" << ","
            << "Azimuth [deg]" << ","
            << "Elevation [deg]" << endl;

    // 2 hour time period, 10 second intervall
    for (int elapsedSeconds = 0; elapsedSeconds <= 2 * 3600; elapsedSeconds += 10) {
        ECICoordinate satPos, satVel;
        auto targetTime = static_cast<int32_t>(round(elapsedSeconds + secondsEpochToObsPeriodStart));
        sgp4Propagator.calculatePositionAndVelocity(targetTime, satPos, satVel);
        double jdCurrent = jdObsPeriodStart + elapsedSeconds / 86400.0;
        ECICoordinate groundStationCurrentPos = convertGeodeticToECI(geodeticGroundStationPos, jdCurrent);
        ECICoordinate rObsSat = {
            satPos.x - groundStationCurrentPos.x,
            satPos.y - groundStationCurrentPos.y,
            satPos.z - groundStationCurrentPos.z
        };
        SEZCoordinate sezCurrentPos = transformECIToSEZ(rObsSat, geodeticGroundStationPos, jdCurrent);
        myFile << jdCurrent << ","
                << satPos.x << ","
                << satPos.y << ","
                << satPos.z << ","
                << groundStationCurrentPos.x << ","
                << groundStationCurrentPos.y << ","
                << groundStationCurrentPos.z << ","
                << sezCurrentPos.rS << ","
                << sezCurrentPos.rE << ","
                << sezCurrentPos.rZ << ","
                << computeSlantRange(sezCurrentPos) << ","
                << rad2deg(computeAzimuth(sezCurrentPos)) << ","
                << rad2deg(computeElevation(sezCurrentPos)) << endl;
    }
    myFile.close();

    // Aufgabe 3.3
    GregorianCalendar ObsDayStartDate{2025, 6, 20, 0, 0, 0};
    double jdObsDayStart = computeJDFromGregCal(ObsDayStartDate);
    double secondsEpochToObsDayStart = (jdObsDayStart - jdEpochOfOMM) * 86400.0;
    bool inReach = false;
    bool aos = false;
    double tempObsStart = 0.0;
    double tempObsEnd = 0.0;

    myFile.open("contact-data.csv", ios::out);
    if (!myFile.is_open()) {
        cout << "Could not open file" << endl;
        return 1;
    }
    myFile << fixed << setprecision(10);
    myFile << "Acquisition of Signal [dd.MM.yyyy hh:mm:ss UTC]" << ","
            << "Loss of Signal [dd.MM.yyyy hh:mm:ss UTC]" << ","
            << "Duration of Contact [mm:ss]" << endl;

    // 24 hour time period, 1 second intervall
    for (int elapsedSeconds = 0; elapsedSeconds <= 24 * 3600; elapsedSeconds += 1) {
        auto targetTime = static_cast<int32_t>(round(elapsedSeconds + secondsEpochToObsDayStart));
        double jdCurrent = jdObsDayStart + elapsedSeconds / 86400.0;
        ECICoordinate satPos, satVel;
        sgp4Propagator.calculatePositionAndVelocity(targetTime, satPos, satVel);
        ECICoordinate groundStationCurrentPos = convertGeodeticToECI(geodeticGroundStationPos, jdCurrent);
        ECICoordinate rObsSat = {
            satPos.x - groundStationCurrentPos.x,
            satPos.y - groundStationCurrentPos.y,
            satPos.z - groundStationCurrentPos.z
        };
        SEZCoordinate sezCurrentPos = transformECIToSEZ(rObsSat, geodeticGroundStationPos, jdCurrent);
        double sezCurrentElevation = rad2deg(computeElevation(sezCurrentPos)); // in degree
        if (sezCurrentElevation >= 2.0) {
            inReach = true;
            // set start date
            if (aos == false) {
                aos = true;
                tempObsStart = jdCurrent;
            }
        } else if (inReach == true) {
            // set end date
            inReach = false;
            aos = false;
            tempObsEnd = jdCurrent;
        }
        if (inReach == false && aos == false && tempObsStart < tempObsEnd) {
            GregorianCalendar gregObsStart = computeGregCalFromJD(tempObsStart);
            GregorianCalendar gregObsEnd = computeGregCalFromJD(tempObsEnd);
            // calculate contact duration in format mm:ss
            int diffTotalSeconds = static_cast<int>(round((tempObsEnd - tempObsStart) * 86400.0));
            int diffObsMin = diffTotalSeconds / 60;
            int diffObsSec = diffTotalSeconds % 60;
            myFile << std::setw(2) << static_cast<int>(gregObsStart.day) << "."
                    << std::setw(2) << static_cast<int>(gregObsStart.month) << "."
                    << std::setw(4) << static_cast<int>(gregObsStart.year) << " "
                    << std::setw(2) << static_cast<int>(gregObsStart.hour) << ":"
                    << std::setw(2) << static_cast<int>(gregObsStart.min) << ":"
                    << std::setw(2) << static_cast<int>(gregObsStart.sec) << ","
                    << std::setw(2) << static_cast<int>(gregObsEnd.day) << "."
                    << std::setw(2) << static_cast<int>(gregObsEnd.month) << "."
                    << std::setw(4) << static_cast<int>(gregObsEnd.year) << " "
                    << std::setw(2) << static_cast<int>(gregObsEnd.hour) << ":"
                    << std::setw(2) << static_cast<int>(gregObsEnd.min) << ":"
                    << std::setw(2) << static_cast<int>(gregObsEnd.sec) << ","
                    << std::setw(2) << diffObsMin << ":"
                    << std::setw(2) << diffObsSec << endl;
            tempObsStart = 0.0;
            tempObsEnd = 0.0;
        }
    }
    myFile.close();

    /*
    // test dates
    GregorianCalendar ObsStartDate{2025, 6, 20, 15, 00, 00};
    GregorianCalendar ObsEndDate{2025, 6, 20, 17, 00, 00};
    double ObsJdStart = 2460847.12500000;
    double ObsJdEnd = 2460847.2083333335;
    cout << "\nJulianisches Datum:" << endl;
    cout << computeJDFromGregCal(ObsStartDate) << endl;
    cout << computeJDFromGregCal(ObsEndDate) << endl;
    cout << "Gregorianischer Kalender:" << endl;
    computeGregCalFromJD(ObsJdStart).print();
    computeGregCalFromJD(ObsJdEnd).print();

    // test azimuth
    cout << fixed << setprecision(2);
    SEZCoordinate sezTest{-0, -1, 0};
    cout << "\nAzimuth Berechnung mit atan2(rE,-rS):" << endl;
    cout << "rE = " << sezTest.rE << ", " << "rS = " << sezTest.rS << "\t"
            << "Azimuth: " << rad2deg(computeAzimuth(sezTest)) << " [deg]" << endl;
    */
}
