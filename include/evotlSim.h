#ifndef EVOTLSIM_H
#define EVOTLSIM_H

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

class aircraft; 

// Class declarations
class evotlSim {

public:
    evotlSim(int numVehicles, int numChargers, int maxRunTime, const std::string& vehicleCsvPath);
    ~evotlSim();
    evotlSim(const evotlSim&) = delete;
    evotlSim& operator=(const evotlSim&) = delete;

    evotlSim(evotlSim&&) = default;
    evotlSim& operator=(evotlSim&&) = default;

    void initSimulation(const std::string& vehicleCsvPath);
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
    std::vector<std::unique_ptr<aircraft>> fleet;

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