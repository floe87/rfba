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

    OMM omm = satMap[59112]; // sonate
    //OMM omm = satMap[88888];  // testCase

    // Aufgabe 2.1
    int timesMin[] = {0, 360, 720, 1080, 1440};
    SGP4Propagator propagator;
    propagator.setOmm(omm);
    // textCase sample values
    double refValuesDeltas[5][6];
    double refValues[5][6] = {
        {2328.97048951, -5995.22076416, 1719.97067261, 2.91207230, -0.98341546, -7.09081703}, //TSINCE 0
        {2456.10705566, -6071.93853760, 1222.89727783, 2.67938992, -0.44829041, -7.22879231}, //TSINCE 360
        {2567.56195068, -6112.50384522, 713.96397400, 2.44024599, 0.09810869, -7.31995916}, //TSINCE 720
        {2663.09078980, -6115.48229980, 196.39640427, 2.19611958, 0.65241995, -7.36282432}, //TSINCE 1080
        {2742.55133057, -6079.67144775, -326.38095856, 1.94850229, 1.21106251, -7.35619372}, //TSINCE 1440
    };
    cout << fixed << setprecision(8);
    cout << "SGP4 propagation results for: " << omm.getSatelliteName() << endl;
    cout << "TSINCE [min]\t"
            "X [km]\t\t"
            "Y [km]\t\t"
            "Z [km]\t\t"
            "XDOT [km/s]\t"
            "YDOT [km/s]\t"
            "ZDOT [km/s]" << endl;
    for (int i = 0; i < 5; i++) {
        ECICoordinate pos;
        ECICoordinate vel;
        int secsAfterEpoch = timesMin[i] * 60;
        propagator.calculatePositionAndVelocity(secsAfterEpoch, pos, vel);
        cout << timesMin[i] << "\t\t"
                << pos.x << "\t"
                << pos.y << "\t"
                << pos.z << "\t"
                << vel.x << "\t"
                << vel.y << "\t"
                << vel.z << endl;

        // calculate Deltas
        if (omm.getSatelliteName() == "TESTCASE") {
            refValuesDeltas[i][0] = pos.x - refValues[i][0];
            refValuesDeltas[i][1] = pos.y - refValues[i][1];
            refValuesDeltas[i][2] = pos.z - refValues[i][2];
            refValuesDeltas[i][3] = vel.x - refValues[i][3];
            refValuesDeltas[i][4] = vel.y - refValues[i][4];
            refValuesDeltas[i][5] = vel.z - refValues[i][5];
        }
    }
    if (omm.getSatelliteName() == "TESTCASE") {
        cout << "\nDeltas:" << endl;
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
    return 0;
}
