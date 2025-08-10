//
// Created by William on 8/9/2025.
//

#ifndef EVOTL_SIM_STATISTICSRECORDER_H
#define EVOTL_SIM_STATISTICSRECORDER_H

#include "aircraftType.hpp"
#include <map>
#include <string>
#include <iostream>
#include <iomanip>
#include "aircraft.h"

struct stats {
    /*  - average flight time per flight
        - average distance traveled per flight
     */
    double totalFlightTime{};
    double totalDistanceTravelled{};
    int flightsCompleted{};

    /* - average time charging per charge session
     */
    double timeSpentCharging{};
    int chargeSessionsCompleted{};

    double totalNumFaults{};
    double totalPassengerMiles{};
};

class statisticsRecorder {
public:
    explicit statisticsRecorder(const std::vector<aircraftType>& types);

    void recordFlight(const aircraftType& type, double distance, double duration);

    void recordChargeSession(const aircraftType& type, double duration);

    void recordFault(const aircraftType& type);

    void displayStats() const;

private:
    std::map<std::string, stats> statsMap;
};
#endif //EVOTL_SIM_STATISTICSRECORDER_H