#include <iostream>
#include "evotlSim.h"

int main() {
    std::cout << "Hello, evotl sim!!" << std::endl;

    // create sim here - numVehicles, numChargers, maxRunTime(seconds), seed,
    evotlSim sim(20, 3, 3);
    sim.startSimulation();

    return 0;
}