#ifndef EVOTLSIM_H
#define EVOTLSIM_H

#include "aircraft.h"
#include "aircraftType.hpp"
#include "charger.h"
#include "statisticsRecorder.h"
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <random>
#include <ctime> // Used for seeding the random number generator
#include <memory>
#include <queue>
#include <set>
#include <filesystem>
#include <algorithm>


// Class declarations
class evotlSim {

public:
    evotlSim(int numVehicles, int numChargers, int maxRunTime) : numVehicles(numVehicles), numChargers(numChargers),
                                                            maxRunTime(maxRunTime),
                                                            random_engine(std::time(nullptr)),
                                                                uniform_dist(0.0, 1.0)
        {};

    ~evotlSim();
    evotlSim(const evotlSim&) = delete;
    evotlSim& operator=(const evotlSim&) = delete;

    void initSimulation();
    void startSimulation();

    void showAircraftTypes() const;
    void showFleet() const;

    //TODO: probably would do private here too but working around this for testing
protected:

    // init variables
    int numVehicles, numChargers, maxRunTime;
    std::mt19937 random_engine;
    std::uniform_real_distribution<double> uniform_dist;


    void initTypesFromCsv(const std::string& fileName);
    std::vector<aircraftType> aircraftTypes;

    void generateFleet();
    std::vector<aircraft> fleet;

    void instantiateChargers();
    std::vector<charger> chargers;
    std::queue<aircraft*> aircraftQueue;

    //ASSUMPTION: using seconds for discrete simulation time
    void runSimulation();

    void handleStep(double timeElapsed);

    bool checkFault(const aircraft &currentAircraft, double timeElapsed);

    statisticsRecorder statisticsRecorder;
};

#endif // EVOTLSIM_H