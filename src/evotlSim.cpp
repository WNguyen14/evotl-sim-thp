#include "../include/evotlSim.h"

#include <algorithm>

//TODO FIX INTS

void evotlSim::startSimulation() {
    //Initialize simulation here, aircraft types, total fleet, chargers
    initTypesFromCsv("../vehicles.csv");
    generateFleet();
    runSimulation(maxRunTime);
}

// max run time is in hours
void evotlSim::runSimulation(int maxRunTime) {

    //TODO multi thread here maybe?

    //convert hours to seconds
    for (uint32_t i = 0; i < maxRunTime * 3600; i++) {
        handleStep(1);
    }

    //END ALL FLYING FLIGHTS
    for (auto& aircraft : fleet) {
        if (aircraft.getAircraftState() == aircraft::FLYING) {
            statisticsRecorder.recordFlight(
                           aircraft.getType(),
                           aircraft.getCurrentFlightDistance(),
                   aircraft.getCurrentFlightTime());
        }
    }

    std::cout << "\n--- END OF SIMULATION ---\n";
    showFleet();
    statisticsRecorder.displayStats();

}

/*
 * assume that a flight begins when simulation starts or when an aircraft leaves the charger
 * and flight ends when simulation ends or aircraft runs out of battery
 */
void evotlSim::handleStep(double timeElapsed) {

    //grab currently flying aircraft
    std::vector<aircraft*> currentlyFlying;
    for (auto& aircraft : fleet) {
        if (aircraft.getAircraftState() == aircraft::FLYING) {
            currentlyFlying.push_back(&aircraft);
        }
    }

    //handle the charging (because you can leave the charger and then join the flying aircraft)
    for (auto& charger : chargers) {
        if (!charger.getIsBusy()) continue;

        aircraft* currentAircraft = charger.getCurrentAircraft();
        double chargeTime = currentAircraft->updateCharge(timeElapsed);

        //if we're now flying, that means we have finished charging for this charger
        if (currentAircraft->getAircraftState() == aircraft::FLYING) {
            // CHARGE EVENT JUST ENDED, LOG THIS CHARGE SESSION
            statisticsRecorder.recordChargeSession(currentAircraft->getType(), currentAircraft->getCurrentChargeSessionTime());

            double remainingTime = timeElapsed - chargeTime;
            if (remainingTime > 0.0) {
                // FLIGHT EVENT STARTS SINCE WE HAVE SOME TIME LEFT IN THIS TIMESTEP
                currentAircraft->startNewFlight();
                double flightTime = currentAircraft->updateFlight(remainingTime);
                if (flightTime > 0.0) {
                    if (checkFault(*currentAircraft, flightTime)) {
                        //ASSUME IT'S OK FOR AIRCRAFT TO KEEP FLYING
                        statisticsRecorder.recordFault(currentAircraft->getType());
                    }
                }
            }

            charger.stopCharging();
        }
    }

    //handling the currently flying aircraft in the fleet
    for (auto& currentAircraft : currentlyFlying) {

        /*
         * thinking about whether the simulation itself should modulate the aircraft states directly,
         * or if the aircraft should mutate their own members. going in the direction of the simulation mutating each
         * aircraft's member variables
         */

        if (currentAircraft->getAircraftState() == aircraft::FLYING)
        {
                // check how much we flew in this timestep
                double flightTime = currentAircraft->updateFlight(timeElapsed);
                if (flightTime > 0.0) {
                    // handling the fault outside the aircraft because seems like the sim should own the occurrence of errors
                    if (checkFault(*currentAircraft, flightTime)) {
                        //ASSUME IT'S OK FOR AIRCRAFT TO KEEP FLYING
                        statisticsRecorder.recordFault(currentAircraft->getType());
                    }
                }
                //check again to see if we had changed states
                if (currentAircraft->getAircraftState() == aircraft::aircraftState::WAITING) {
                    statisticsRecorder.recordFlight(
                            currentAircraft->getType(),
                            currentAircraft->getCurrentFlightDistance(),
                    currentAircraft->getCurrentFlightTime()
                    );
                    aircraftQueue.push(currentAircraft);
                }
        }
    }

    // handle the waiting aircraft and let them charge
    for (auto& charger : chargers) {
        if (!charger.getIsBusy() && !aircraftQueue.empty()) {
            aircraft* nextAircraft = aircraftQueue.front();
            aircraftQueue.pop();

            nextAircraft->startNewChargeSession();
            charger.startCharging(nextAircraft);

        }
    }

}

std::string aircraftStateToString(aircraft::aircraftState state) {
    switch (state) {
        case aircraft::FLYING:   return "Flying";
        case aircraft::CHARGING: return "Charging";
        case aircraft::WAITING:  return "Waiting for Charger";
        default:                 return "Unknown";
    }
}

void evotlSim::showFleet() const {
    std::cout << "\n--- FLEET STATUS ---\n";
    if (fleet.empty()) {
        std::cout << "The fleet is empty.\n";
        return;
    }

    // A simple header for the output table
    std::cout << std::left << std::setw(20) << "Aircraft Type"
              << "| State\n";
    std::cout << "--------------------|----------------------\n";

    // Loop through each vehicle in the fleet.
    // Use 'const auto&' for efficiency to avoid copying the aircraft object.
    for (const auto& vehicle : fleet) {

        // Get the vehicle's type name
        std::string name = vehicle.getType().getName();

        // Get the vehicle's state and convert it to a string using our helper
        std::string state_str = aircraftStateToString(vehicle.getAircraftState());

        // Print the formatted line
        std::cout << std::left << std::setw(20) << name
                  << "| " << state_str << '\n';
    }
    std::cout << "--------------------\n";
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

        aircraftType temp{aircraftType(name, cruise, batt, charge, energy, passengers, fault)};

        aircraftTypes.push_back(temp);
    }
}

//generate many aircraft in this fleet such that we have at least one in the fleet per type of vehicle
void evotlSim::generateFleet() {

    //create random integers from 0 -> num aircraft types
    //TODO probably OK to use size_t here for the second param
    // ASSUMPTION: do not need to guarantee that at least one company is represented? will put in at least one of each
    // for completeness though
    std::uniform_int_distribution<int> distribution(0, aircraftTypes.size() - 1);

    //fill in one aircraft for each aircraft type
    for (const auto & aircraftType : aircraftTypes) {
        fleet.emplace_back(aircraft(aircraftType));
    }
    //fill in the remainder
    int remainder = numVehicles - fleet.size();
    for (int i = 0; i < remainder; ++i) {
        int random_index = distribution(random_engine);
        fleet.emplace_back(aircraftTypes[random_index]);
    }

}

void evotlSim::instantiateChargers() {
    for (int i = 0; i < numChargers; ++i) {
        chargers.emplace_back();
    }
}


// assuming its OK to divide the probability of fault from hours to seconds
bool evotlSim::checkFault(const aircraft &currentAircraft, double timeElapsed) {
    double probability = currentAircraft.getType().getFaultProbability();

    double convertToHours = timeElapsed / 3600.0;
    double probabilityNoFault = 1.0 - probability;
    double probabilityInStep = std::pow(probabilityNoFault, convertToHours);
    double probabilityFault = 1.0 - probabilityInStep;


    double rollForFault = uniform_dist(random_engine);

    return rollForFault < probabilityFault;
}


