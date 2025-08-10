#include <iostream>
#include "evotlSim.h"

int main() {
    // create sim here - numVehicles, numChargers, maxRunTime(seconds), seed,
    evotlSim sim(20, 3, 3);
    sim.startSimulation();

    return 0;
}

// current issues:
/*
 *  division is super weird sometimes
 */