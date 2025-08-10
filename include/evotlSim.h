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
#include <queue>


// Class declarations
class evotlSim {

public:
    evotlSim(int numVehicles, int numChargers, int maxRunTime) : numVehicles(numVehicles), chargers(numChargers),
                                                                 maxRunTime(maxRunTime),
                                                                 random_engine(std::time(nullptr)),
                                                                 statisticsRecorder(std::vector<aircraftType>(numVehicles)){
    };

    void startSimulation();

    void showAircraftTypes() const;
private:

    // init variables
    int numVehicles, numChargers, maxRunTime;
    std::mt19937 random_engine;


    void initTypesFromCsv(const std::string& fileName);
    std::vector<aircraftType> aircraftTypes;

    void generateFleet();
    std::vector<aircraft> fleet;

    void instantiateChargers();
    std::vector<charger> chargers;
    std::queue<aircraft*> aircraftQueue;

    //ASSUMPTION: using seconds for discrete simulation time
    void runSimulation(int maxRunTime);
    int currentTime{};

    void handleStep(double timeElapsed);

    bool checkFault(const aircraft &currentAircraft, double timeElapsed);

    statisticsRecorder statisticsRecorder;
};

#endif // EVOTLSIM_H