//
// Created by William on 8/8/2025.
//

#ifndef EVOTL_SIM_AIRCRAFTTYPE_H
#define EVOTL_SIM_AIRCRAFTTYPE_H
#include <string>
#include <sstream>
class aircraftType {
public:

    aircraftType() = default;

    aircraftType(std::string name, const double cruiseSpeed, const double batteryCapacity, const double timeToCharge,
        const double energyUsage, const int passengerCount, const double faultProbability) {
        this->name = name;
        this->cruiseSpeed = cruiseSpeed;
        this->batteryCapacity = batteryCapacity;
        this->timeToCharge = timeToCharge;
        this->energyUsage = energyUsage;
        this->passengerCount = passengerCount;
        this->faultProbability = faultProbability;
    };



    std::string getName() const {return name;}
    double getCruiseSpeed() const { return cruiseSpeed; }
    double getBatteryCapacity() const { return batteryCapacity; }
    double getTimeToCharge() const { return timeToCharge; }
    double getEnergyUsage() const { return energyUsage; }
    int getPassengerCount() const { return passengerCount; }
    double getFaultProbability() const { return faultProbability; }

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
    std::string name{"NOT INITIALIZED"};

    //TODO maybe make a null type, for now just default everything to 0
    double cruiseSpeed{};
    double batteryCapacity{};
    double timeToCharge{};
    double energyUsage{};
    int passengerCount{};
    double faultProbability{};
};

#endif //EVOTL_SIM_AIRCRAFTTYPE_H