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
    // File with TLEs
    string fileName = "omm.txt";
    //cout << "Test" << endl;
    // Get Map from txt file
    map<int, OMM> satMap = readOmmFile(fileName.c_str());
    //OMM omm = satMap[12345];	// Beispiel

    OMM omm = satMap[59112]; // SONATE-2
    //OMM omm = satMap[88888]; // TESTCASE
    //omm.print();
    SGP4Propagator propagator;
    propagator.setOmm(omm);
    cout << fixed << setprecision(8);

    // Aufgabe 2.1
    int timesMin[] = {0, 360, 720, 1080, 1440};
    // textCase sample values
    double refValuesDeltas[5][6];
    double refValues[5][6] = {
        {2328.97048951, -5995.22076416, 1719.97067261, 2.91207230, -0.98341546, -7.09081703}, //TSINCE 0
        {2456.10705566, -6071.93853760, 1222.89727783, 2.67938992, -0.44829041, -7.22879231}, //TSINCE 360
        {2567.56195068, -6112.50384522, 713.96397400, 2.44024599, 0.09810869, -7.31995916}, //TSINCE 720
        {2663.09078980, -6115.48229980, 196.39640427, 2.19611958, 0.65241995, -7.36282432}, //TSINCE 1080
        {2742.55133057, -6079.67144775, -326.38095856, 1.94850229, 1.21106251, -7.35619372}, //TSINCE 1440
    };
    cout << "\nSGP4 propagation results for: " << omm.getSatelliteName() << endl;
    cout << "TSINCE [min]\t"
            "X [km]\t\t"
            "Y [km]\t\t"
            "Z [km]\t\t"
            "XDOT [km/s]\t"
            "YDOT [km/s]\t"
            "ZDOT [km/s]" << endl;
    for (int i = 0; i < 5; i++) {
        ECICoordinate position;
        ECICoordinate velocity;
        int secondsAfterEpoch = timesMin[i] * 60;
        propagator.calculatePositionAndVelocity(secondsAfterEpoch, position, velocity);
        cout << timesMin[i] << "\t\t"
                << position.x << "\t"
                << position.y << "\t"
                << position.z << "\t"
                << velocity.x << "\t"
                << velocity.y << "\t"
                << velocity.z << endl;
        // calculate Deltas
        if (omm.getSatelliteName() == "TESTCASE") {
            refValuesDeltas[i][0] = position.x - refValues[i][0];
            refValuesDeltas[i][1] = position.y - refValues[i][1];
            refValuesDeltas[i][2] = position.z - refValues[i][2];
            refValuesDeltas[i][3] = velocity.x - refValues[i][3];
            refValuesDeltas[i][4] = velocity.y - refValues[i][4];
            refValuesDeltas[i][5] = velocity.z - refValues[i][5];
        }
    }
    if (omm.getSatelliteName() == "TESTCASE") {
        cout << "\nDeltas to reference values of TESTCASE:" << endl;
        cout << "TSINCE [min]\t"
                "dX [km]\t\t"
                "dY [km]\t\t"
                "dZ [km]\t\t"
                "dXDOT [km/s]\t"
                "dYDOT [km/s]\t"
                "dZDOT [km/s]" << endl;
        for (int i = 0; i < 5; i++) {
            cout << timesMin[i] << "\t\t"
                    << refValuesDeltas[i][0] << "\t"
                    << refValuesDeltas[i][1] << "\t"
                    << refValuesDeltas[i][2] << "\t"
                    << refValuesDeltas[i][3] << "\t"
                    << refValuesDeltas[i][4] << "\t"
                    << refValuesDeltas[i][5] << endl;
        }
    }

    // Aufgabe 2.2
    fstream myFile;
    myFile.open("SGP4-propagation-data.csv", ios::out);
    if (!myFile.is_open()) {
        cout << "Could not open file" << endl;
        return 1;
    }
    myFile << "Elapsed Time since Epoch[min]" << ","
            << "ECI: X[km]" << ","
            << "ECI: Y[km]" << ","
            << "ECI: Z[km]" << ","
            << "Julian Date[days]" << ","
            << "GMST[deg]" << ","
            << "Geocentric Latitude[deg]" << ","
            << "Geocentric Longitude[deg]" << ","
            << "Geocentric Height[km]" << ","
            << "Geodetic Latitude[deg]" << ","
            << "Geodetic Longitude[deg]" << ","
            << "Geodetic Height[km]" << endl;

    // calculate julian Date to OMM Epoch
    double jd = computeJD(omm.getYear(), omm.getDayFraction());
    double orbitPeriodInMin = omm.calcOrbitPeriod() / 60.0;
    cout << "\nOrbit period [min]: " << orbitPeriodInMin << endl;
    myFile << fixed << setprecision(8);

    // 100 min period; 5 min intervall
    //for (int elapsedTimeInMin = 0; elapsedTimeInMin <= 100; elapsedTimeInMin += 1) {
    // 3 * Orbit period; 1 min intervall
    for (int elapsedTimeInMin = 0; elapsedTimeInMin <= 3 * orbitPeriodInMin; elapsedTimeInMin += 1) {
        ECICoordinate position{}, velocity{};
        propagator.calculatePositionAndVelocity(elapsedTimeInMin * 60, position, velocity);
        double jdCurrent = jd + elapsedTimeInMin * 1 / (24.0 * 60.0);
        double gmstCurrent = computeGMST(jdCurrent);
        GeocentricCoordinate geocentricCoordinate = convertECItoGeocentric(position, jdCurrent);
        GeodeticCoordinate geodeticCoordinate = convertECItoGeodetic(position, jdCurrent);
        myFile << elapsedTimeInMin << ","
                << position.x << ","
                << position.y << ","
                << position.z << ","
                << jdCurrent << ","
                << rad2deg(gmstCurrent) << ","
                << rad2deg(geocentricCoordinate.latitude) << ","
                << rad2deg(geocentricCoordinate.longitude) << ","
                << geocentricCoordinate.height << ","
                << rad2deg(geodeticCoordinate.latitude) << ","
                << rad2deg(geodeticCoordinate.longitude) << ","
                << geodeticCoordinate.height << endl;
    }
    myFile.close();
}
