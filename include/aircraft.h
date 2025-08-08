//
// Created by William on 8/8/2025.
//

#ifndef EVOTL_SIM_AIRCRAFT_H
#define EVOTL_SIM_AIRCRAFT_H

class Aircraft {
public:
    Aircraft(const float cruiseSpeed, const float batteryCapacity, const float timeToCharge,
        const float energyUsage, const int passengerCount, const float faultProbability) {
        this->cruiseSpeed = cruiseSpeed;
        this->batteryCapacity = batteryCapacity;
        this->timeToCharge = timeToCharge;
        this->energyUsage = energyUsage;
        this->passengerCount = passengerCount;
        this->faultProbability = faultProbability;
    }
    ~Aircraft();

    float getCruiseSpeed() const { return cruiseSpeed; }
    float getBatteryCapacity() const { return batteryCapacity; }
    float getTimeToCharge() const { return timeToCharge; }
    float getEnergyUsage() const { return energyUsage; }
    int getPassengerCount() const { return passengerCount; }
    float getFaultProbability() const { return faultProbability; }

    //not writing setters because assuming can't change the characters of the aircraft after you create it

private:
    float cruiseSpeed;
    float batteryCapacity;
    float timeToCharge;
    float energyUsage;
    int passengerCount;
    float faultProbability;

};


#endif //EVOTL_SIM_AIRCRAFT_H