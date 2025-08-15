#include "../include/evotlSim.h"
#include "../include/aircraft.h"


evotlSim::evotlSim(int numVehicles, int numChargers, int maxRunTime, const std::string& vehicleCsvPath) : 
    numVehicles(numVehicles), 
    numChargers(numChargers),
    maxRunTime(maxRunTime),
    random_engine(std::time(nullptr)),
    uniform_dist(0.0, 1.0)
{
    // The constructor now immediately initializes the simulation
    initSimulation(vehicleCsvPath);
}

evotlSim::~evotlSim() {
    // When the simulation is over, we must ensure no chargers are left
    // holding pointers to aircraft that are about to be destroyed.
    for (auto& charger : chargers) {
        if (charger.getIsBusy()) {
            charger.stopCharging(); // This sets currentAircraft to nullptr
        }
    }
}

void evotlSim::initSimulation(const std::string& vehicleCsvPath) {
    initTypesFromCsv(vehicleCsvPath);
    
    if (aircraftTypes.empty()) {
        std::cerr << "ERROR: No aircraft types were loaded from the CSV file. Aborting." << std::endl;
        abort();
    }
    
    statisticsRecorder.init(aircraftTypes);
                  
    generateFleet();
    instantiateChargers();
}

void evotlSim::startSimulation() {
    //Initialize simulation here, aircraft types, total fleet, chargers
    runSimulation();
    std::cout << "[DEBUG] Returned from runSimulation SUCCESSFULLY." << std::endl;

}

// max run time is in hours
void evotlSim::runSimulation() {

    //TODO multi thread here maybe?
    std::cout << "[DEBUG] Starting simulation loop..." << std::endl;

    //convert hours to seconds
    for (uint32_t i = 0; i < maxRunTime * 3600; i++) {
        if (i % 1000 == 0) { // Print a message every 1000 steps
            std::cout << "[DEBUG] Simulation step " << i << std::endl;
        }
        handleStep(1);
    }
    std::cout << "[DEBUG] Simulation loop FINISHED." << std::endl;

    //END ALL FLYING FLIGHTS
    for (const auto& aircraft : fleet) {
        if (aircraft->getAircraftState() == aircraft::FLYING) {
            statisticsRecorder.recordFlight(
                        aircraft->getType(),
                        aircraft->getCurrentFlightDistance(),
                        aircraft->getCurrentFlightTime());
        }
    }

    //END ALL CHARGING SESSIONS
    for (auto& charger : chargers) {
        if (charger.getIsBusy()) {
            aircraft* currentAircraft = charger.getCurrentAircraft();
            // Only record if they actually spent time charging in this session
            if (currentAircraft->getCurrentChargeSessionTime() > 0.0) {
                statisticsRecorder.recordChargeSession(
                    currentAircraft->getType(),
                    currentAircraft->getCurrentChargeSessionTime()
                );
            }
        }
    }

    std::cout << "\n--- END OF SIMULATION ---\n";

    showFleet();


    statisticsRecorder.displayStats();

}

/*
 * assume that a flight begins when simulation starts or when an aircraft leaves the charger
 * and flight ends when simulation ends or aircraft runs out of battery
 *
 * !!! assuming that an aircraft that doesn't have enough battery for one second of movement does not exist
 */
void evotlSim::handleStep(double timeElapsed) {

    std::set<aircraft*> aircraftHandledThisStep;

    //grab currently flying aircraft
    std::vector<aircraft*> currentlyFlying;
    for (const auto& aircraft : fleet) {
        if (aircraft->getAircraftState() == aircraft::FLYING) {
            currentlyFlying.push_back(aircraft.get());
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
            aircraftHandledThisStep.insert(currentAircraft);

        }
    }

    //handling the currently flying aircraft in the fleet
    for (auto& currentAircraft : currentlyFlying) {

        if (aircraftHandledThisStep.count(currentAircraft)) {
            continue; // Skip this aircraft, its logic was handled in the charger loop
        }
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
                            currentAircraft->getCurrentFlightTime());
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
    for (const auto& aircraft : fleet) {

        // Get the vehicle's type name
        std::string name = aircraft->getType().getName();

        // Get the vehicle's state and convert it to a string using our helper
        std::string state_str = aircraftStateToString(aircraft->getAircraftState());

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
    if (!file.is_open()) {
        std::cerr << "ERROR: Could not open file: " << fileName << std::endl;
        abort();
    }
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

void evotlSim::generateFleet() {
    // Ensure the fleet is empty before we start.
    fleet.clear(); 

    // This check is important to prevent a crash if the CSV is empty.
    if (aircraftTypes.empty()) {
        std::cerr << "ERROR: Cannot generate fleet, no aircraft types available." << std::endl;
        return;
    }

    // This distribution is now safe because we've checked that aircraftTypes is not empty.
    std::uniform_int_distribution<int> distribution(0, aircraftTypes.size() - 1);

    // First, guarantee that we have at least one of each aircraft type,
    // as long as we don't exceed the total number of vehicles requested.
    if (numVehicles >= static_cast<int>(aircraftTypes.size())) {
        for (const auto& type : aircraftTypes) {
            // Correctly create a unique_ptr and move it into the vector.
            // push_back is generally safer and clearer than emplace_back with smart pointers.
            fleet.push_back(std::make_unique<aircraft>(type));
        }
    }

    // Then, fill the remainder of the fleet with random aircraft
    // until we reach the desired number of vehicles.
    while (fleet.size() < numVehicles) {
        int random_index = distribution(random_engine);
        fleet.push_back(std::make_unique<aircraft>(aircraftTypes[random_index]));
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


