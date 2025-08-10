//
// Created by William on 8/8/2025.
//

#ifndef EVOTL_SIM_AIRCRAFT_H
#define EVOTL_SIM_AIRCRAFT_H

#include "aircraftType.hpp"
#include <random>
#include <cmath> // For std::pow

class aircraft {
    static constexpr double SEC_TO_HOURS = 1.0/3600.0;
public:
    explicit aircraft(const aircraftType &specification) {
        type = specification;
        currentCharge = type.getBatteryCapacity();
        currentState = aircraftState::FLYING;
    }
    ~aircraft() = default;

    aircraftType getType() const {return type;}
    double getCurrentCharge() const {return currentCharge;}
    double getCurrentFlightTime() const{return currentFlightTime;};
    double getCurrentFlightDistance() const {return currentFlightDistance;};
    double getCurrentChargeSessionTime() const {return currentChargeSessionTime;};

    //can add catastrophic state maybe
    enum aircraftState {
        FLYING,
        CHARGING,
        WAITING
    };
    aircraftState getAircraftState() const {return currentState;}

    double updateFlight(double timeElapsed);
    double updateCharge(double timeElapsed);

    void startNewFlight();
    void startNewChargeSession();


private:
    //TODO create default type maybe
    aircraftType type{};

    // aircraft state - they start with max battery and already flying
    double currentCharge{};
    double currentFlightTime{};
    double currentFlightDistance{};
    double currentChargeSessionTime{};
    aircraftState currentState{};

    //implement current velocity for future work - instead of constantly using max speed, implement with acceleration



};


#endif //EVOTL_SIM_AIRCRAFT_H