//
// Created by William on 8/8/2025.
//

#ifndef EVOTL_SIM_AIRCRAFT_H
#define EVOTL_SIM_AIRCRAFT_H

#include "aircraftType.hpp"
class aircraft {
public:
    aircraft(const aircraftType& specification) {
       type = specification;
    }
    ~aircraft() = default;

    aircraftType getType() const {return type;}


private:
    aircraftType type{};

    double totalFlightTime{};
    double totalDistanceTravelled{};
    double timeSpentCharging{};
    double totalNumFaults{};
    double totalPassengerMiles{};

};


#endif //EVOTL_SIM_AIRCRAFT_H