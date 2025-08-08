#include "evotlSim.h"

#include <algorithm>

// main simulation runner
evotlSim::evotlSim(int seed) {
    initTypesFromCsv("../vehicles.csv");
    generateFleet(20, seed);
}

void evotlSim::showAircraftTypes() const {
    for (const auto & aircraftType : aircraftTypes) {
        std::cout << aircraftType.toString() << "\n\n";
    }
}

// TODO: guard against bad files
void evotlSim::initTypesFromCsv(const std::string& fileName) {
    aircraftTypes.clear();
    std::ifstream file(fileName);
    std::string line;
    getline(file, line); // skip header

    while (getline(file, line)) {

        std::stringstream ss(line);
        std:: string name;
        float cruise, batt, charge, energy;
        int passengers;
        float fault;
        char comma;

        getline(ss, name, ',');
        ss >> cruise >> comma >> batt >> comma >> charge >> comma
           >> energy >> comma >> passengers >> comma >> fault;

        auto temp = aircraftType(name, cruise, batt, charge, energy, passengers, fault);

        aircraftTypes.push_back(temp);
    }
}

//generate many aircraft in this fleet such that we have at least one in the fleet per type of vehicle
void evotlSim::generateFleet(const int numberInFleet, const int seed) {

    //create RNG machine
    std::mt19937 random_engine(seed);
    //create random integers from 0 -> num aircraft types
    //TODO probably OK to use size_t here for the second param
    // ASSUMPTION: do not need to guarantee that at least one company is represented? will put in at least one of each
    // for completeness though
    std::uniform_int_distribution<int> distribution(0, aircraftTypes.size());

    //fill in one aircraft for each aircraft type
    for (const auto & aircraftType : aircraftTypes) {
        auto a = aircraft(aircraftType);
        fleet.push_back(a);
    }
    //fill in the remainder
    int remainder = numberInFleet - fleet.size();
    for (int i = 0; i < remainder; ++i) {
        int random_index = distribution(random_engine);
        auto a = aircraft(aircraftTypes[random_index]);
        fleet.push_back(a);
    }

}
