#include "evotlSim.h"

// main simulation runner
evotlSim::evotlSim() {
    initTypesFromCsv("../vehicles.csv");
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
