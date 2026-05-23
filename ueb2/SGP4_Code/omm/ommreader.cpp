#include "ommreader.h"
#include <iostream>
#include <fstream>
#include <string>


map<int, OMM> readOmmFile(const char *fileName) {
	map<int, OMM> satMap;
	ifstream file(fileName);
	string line;
	vector<std::string> currentBlock;
	if (file.is_open()) {
        while (std::getline(file, line)) {
            // Entferne potenzielle Carriage Returns (\r) für sauberes Parsing
            if (!line.empty() && line.back() == '\r') line.pop_back();
			if (line.empty()) continue; // Leere Zeilen überspringen
			
            // Wenn eine neue Sektion beginnt und wir bereits Daten gesammelt haben
            if (line.find("CCSDS_OMM_VERS") != std::string::npos && !currentBlock.empty()) {
                // Erzeuge Objekt mit den Rohdaten
                OMM elements(currentBlock);
				// ID holen (wird intern in populate/getSatID ermittelt)
				int foundID = -1;
				for (const std::string& s : currentBlock) {
					if (s.find("NORAD_CAT_ID") != std::string::npos) {
						size_t sep = s.find('=');
						if (sep != std::string::npos) {
							foundID = std::stoi(s.substr(sep + 1));
						}
						break; 
					}
				}
                satMap.insert({foundID, elements});
                currentBlock.clear();
            }
            
            // Zeile einfach zum aktuellen Block hinzufügen, sofern nicht komplett leer
            if (!line.empty()) {
                currentBlock.push_back(line);
            }
        }
        
        // Den letzten Block nach dem Schleifenende verarbeiten
        if (!currentBlock.empty()) {
            OMM elements(currentBlock);
			int foundID = -1;
			for (const std::string& s : currentBlock) {
					if (s.find("NORAD_CAT_ID") != std::string::npos) {
						size_t sep = s.find('=');
						if (sep != std::string::npos) {
							foundID = std::stoi(s.substr(sep + 1));
						}
						break; 
					}
				}
			satMap.insert({foundID, elements});
        }
        file.close();
    }
    return satMap;
}
