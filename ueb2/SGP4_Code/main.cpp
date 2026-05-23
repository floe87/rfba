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


int main(int argc, char *argv[])
{
	// File with TLEs
	string fileName = "omm.txt";
	//cout << "Test" << endl;
	// Get Map from txt file
	map<int, OMM> satMap = readOmmFile(fileName.c_str());
	//OMM omm = satMap[12345];	// Beispiel

	OMM sonate = satMap[59112];
	sonate.print();
	return 0;
}