#ifndef EVOTLSIM_H
#define EVOTLSIM_H

#include "aircraft.h"
#include "aircraftType.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>

// Class declarations
class evotlSim {

public:
    evotlSim();

    void showAircraftTypes() const;
private:

    void initTypesFromCsv(const std::string& fileName);

    std::vector<aircraftType> aircraftTypes;
};

#endif // EVOTLSIM_H