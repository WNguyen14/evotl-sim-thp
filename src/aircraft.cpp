//
// Created by William on 8/9/2025.
//

#include "../include/aircraft.h"

#include <cmath>

// timeElapsed: SECONDS
double aircraft::updateFlight(double timeElapsed) {
    //edge case: if we're really close to the end of the battery, we need to divide based on energy usage
    //edge case: using exactly the right amount of charge for the time elapsed

    if (currentCharge <= 0) {
        // this means we didn't fly at all
        return 0.0;
    }

    //convert from mph to seconds
    const double speed = type.getCruiseSpeed() * SEC_TO_HOURS;
    const double energyUsage = type.getEnergyUsage();

    //scalar miles travelled in this timestep
    const double distanceForFullStep = speed * timeElapsed;
    const double energyForFullStep = distanceForFullStep * energyUsage;

    double timeFlownInThisStep = 0.0;

    // use all the energy we need for this step
    if (currentCharge >= energyForFullStep) {
        currentCharge -= energyForFullStep;
        currentFlightDistance += distanceForFullStep;
        timeFlownInThisStep = timeElapsed;
    }

    //fractional case
    else {
        const double distanceActuallyFlown = currentCharge / energyUsage;
        const double timeActuallyFlown = distanceActuallyFlown / speed;
        timeFlownInThisStep = timeActuallyFlown;
        currentFlightDistance += distanceActuallyFlown;
        currentCharge = 0.0;
        currentState = aircraftState::WAITING;
    }
    currentFlightTime += timeFlownInThisStep;

    return timeFlownInThisStep;
}

double aircraft::updateCharge(double timeElapsed) {
    //edge case, if we spent the first 50% of this timestep charging, we should take off immediately afterward
    const double batteryCapacity = type.getBatteryCapacity();
    if (currentCharge >= batteryCapacity) {
        // didn't need to charge at all (max battery)
        currentState = aircraftState::FLYING;
        return 0.0;
    }

    // convert to kwh/second
    const double timeToReachFullSeconds = type.getTimeToCharge() / SEC_TO_HOURS;
    const double chargeRateSeconds = batteryCapacity / timeToReachFullSeconds;
    const double energyNeeded = batteryCapacity - currentCharge;
    const double secondsToReachFull = energyNeeded / chargeRateSeconds;

    double timeSpentChargingInThisStep = 0.0;


    if (secondsToReachFull <= timeElapsed) {
        // fractional case
        currentCharge = batteryCapacity;
        timeSpentChargingInThisStep = secondsToReachFull;
        // if we spent less than the total amount of time in this step charging, that means we just finished charging
        currentState = aircraftState::FLYING;
    }
    else {
        // full usage of time step
        currentCharge += chargeRateSeconds * timeElapsed;
        timeSpentChargingInThisStep += timeElapsed;
    }
    currentChargeSessionTime += timeSpentChargingInThisStep;
    return timeSpentChargingInThisStep;

}

void aircraft::startNewFlight() {
    currentFlightTime = 0.0;
    currentFlightDistance = 0.0;
}

void aircraft::startNewChargeSession() {
    currentState = aircraftState::CHARGING;
    currentChargeSessionTime = 0.0;
}





