#ifndef OMMREADER_H_
#define OMMREADER_H_

#include <map>
#include <vector>
#include "omm.h"

using namespace std;

/**
 * @brief Reads OMMs from a txt file and saves them in a map with the satellite ID as key.
 * @param fileName - txt file with OMMs
 * @return Map of Satellite IDs to their OMMs
 */
map<int, OMM> readOmmFile(const char *fileName);
#endif /* OMMREADER_H_ */