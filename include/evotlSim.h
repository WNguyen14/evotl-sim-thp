#ifndef EVOTLSIM_H
#define EVOTLSIM_H

#include "aircraft.h"
#include "aircraftType.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <random>
#include <ctime> // Used for seeding the random number generator

// Class declarations
class evotlSim {

public:
    explicit evotlSim(int seed);

    void showAircraftTypes() const;
private:

    void initTypesFromCsv(const std::string& fileName);
    std::vector<aircraftType> aircraftTypes;

    void generateFleet(const int numberInFleet, const int seed);
    std::vector<aircraft> fleet;
};

#endif // EVOTLSIM_H