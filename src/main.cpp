#include <iostream>
#include "evotlSim.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "ERROR: Incorrect number of arguments." << std::endl;
        std::cerr << "Usage: " << argv[0] << " <path_to_vehicles.csv>" << std::endl;
        return 1; // Return a non-zero value to indicate an error
    }
    std::string csv_path = argv[1];

    // create sim here - numVehicles, numChargers, maxRunTime(hours), seed,
    evotlSim sim(20, 3, 3, csv_path);
    sim.startSimulation();

    return 0;
}

// current issues:
/*
 *  division is super weird sometimes
 *  more granularity with timestepping? (currently pretty tied to one second per time step)
 */