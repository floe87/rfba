#include "ommreader.h"
#include <iostream>
#include <fstream>
#include <string>


map<int, OMM> readOmmFile(const char *fileName) {
    map<int, OMM> satMap;

    std::ifstream OmmFile(fileName);
    if (!OmmFile.is_open()) {
        printf("Cannot find file\n");
        return satMap;
    }

    std::string OmmSingleLine;
    std::vector<string> block;

    // collect data off OMM blocks
    while (std::getline(OmmFile, OmmSingleLine)) {
        if (OmmSingleLine.empty()) {
            continue;
        }
        // got all data of one OMM block
        if (OmmSingleLine.find("CCSDS_OMM_VERS") != std::string::npos && !block.empty()) {
            OMM omm(block);
            satMap.insert(make_pair(omm.getSatID(), omm));
            block.clear();
        }
        block.push_back(OmmSingleLine);
    }
    // collect data of last OMM block
    if (!block.empty()) {
        OMM omm(block);
        satMap.insert(make_pair(omm.getSatID(), omm));
    }
    return satMap;
}
