#include <iostream>
#include "evotlSim.h"

int main() {
    // create sim here - numVehicles, numChargers, maxRunTime(hours), seed,
    evotlSim sim(20, 3, 3);
    sim.initSimulation(); // Initialize aircraft types, fleet, and chargers
    sim.startSimulation();

    return 0;
}

// current issues:
/*
 *  division is super weird sometimes
 *  more granularity with timestepping? (currently pretty tied to one second per time step)
 */