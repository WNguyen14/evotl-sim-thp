//
// Created by William on 8/8/2025.
//

#ifndef EVOTL_SIM_AIRCRAFTTYPE_H
#define EVOTL_SIM_AIRCRAFTTYPE_H
#include <string>
#include <sstream>
class aircraftType {
public:
    aircraftType(std::string name, const float cruiseSpeed, const float batteryCapacity, const float timeToCharge,
        const float energyUsage, const int passengerCount, const float faultProbability) {
        this->name = name;
        this->cruiseSpeed = cruiseSpeed;
        this->batteryCapacity = batteryCapacity;
        this->timeToCharge = timeToCharge;
        this->energyUsage = energyUsage;
        this->passengerCount = passengerCount;
        this->faultProbability = faultProbability;
    };

    std::string getName() const {return name;}
    float getCruiseSpeed() const { return cruiseSpeed; }
    float getBatteryCapacity() const { return batteryCapacity; }
    float getTimeToCharge() const { return timeToCharge; }
    float getEnergyUsage() const { return energyUsage; }
    int getPassengerCount() const { return passengerCount; }
    float getFaultProbability() const { return faultProbability; }

    std::string toString() const {
        std::ostringstream oss;
        oss << "Aircraft Type: " << getName()
            << "\nCruise Speed: " << getCruiseSpeed() << " km/h"
            << "\nBattery Capacity: " << getBatteryCapacity() << " kWh"
            << "\nTime to Charge: " << getTimeToCharge() << " h"
            << "\nEnergy Usage: " << getEnergyUsage() << " kWh/km"
            << "\nPassenger Count: " << getPassengerCount()
            << "\nFault Probability: " << getFaultProbability();
        return oss.str();
    }

private:
    std::string name;
    float cruiseSpeed;
    float batteryCapacity;
    float timeToCharge;
    float energyUsage;
    int passengerCount;
    float faultProbability;
};

#endif //EVOTL_SIM_AIRCRAFTTYPE_H