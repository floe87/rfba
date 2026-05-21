#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "omm.h"
#include "ommreader.h"

using namespace std;

int main(int argc, char *argv[]) {
    // File with OMMs
    string fileName = "cubesat.txt";

    // Get Map from txt file
    map<int, OMM> satMap = readOmmFile(fileName.c_str());

    // AUFGABE 1.1
    std::size_t numOfElem = satMap.size();
    std::size_t numOfValidElem = 0;
    std::size_t numOfInvalidElem = 0;
    for (auto &elem: satMap) {
        if (elem.second.getIsValid()) {
            numOfValidElem++;
        } else numOfInvalidElem++;
    }
    printf("-------------------------------------------\n");
    printf("Number of entries:\t%zu\n", numOfElem);
    printf("Valid entries:\t\t%zu\n", numOfValidElem);
    printf("Invalid entries:\t%zu\n", numOfInvalidElem);
    printf("-------------------------------------------\n");

    // print entry with ID
    int searchIDs[] = {59112}; // SONATE-2
    for (int id: searchIDs) {
        if (auto tempEntry = satMap.find(id); tempEntry != satMap.end()) {
            printf("NORAD-CAT-ID:\t\t%d\n", id);
            tempEntry->second.print();
        } else printf("Entry not found with NORAD-CAT-ID: %d\n", id);
        printf("-------------------------------------------\n");
    }

    /*
    // loop through every entry
    int count = 1;
    for (auto &elem: satMap) {
        printf("%d. Entry:\n", count++);
        elem.second.print();
        printf("-------------------------------------------\n");
    }
    */
}
