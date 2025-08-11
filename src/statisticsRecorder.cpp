//
// Created by William on 8/10/2025.
//

#include "../include/statisticsRecorder.h"

statisticsRecorder::statisticsRecorder(const std::vector<aircraftType>& types) {
    for (const auto & type : types) {
        statsMap[type.getName()] = stats();
    }
}

void statisticsRecorder::recordFlight(const aircraftType& type, double distance, double duration) {
    auto& stat = statsMap[type.getName()];
    stat.flightsCompleted++;
    stat.totalDistanceTravelled += distance;
    stat.totalFlightTime+=duration;
    stat.totalPassengerMiles+= (type.getPassengerCount() * distance);
}

void statisticsRecorder::recordChargeSession(const aircraftType& type, double duration) {
    auto& stat = statsMap[type.getName()];
    stat.chargeSessionsCompleted++;
    stat.timeSpentCharging += duration;
}

void statisticsRecorder::recordFault(const aircraftType& type) {
    statsMap[type.getName()].totalNumFaults++;
}

void statisticsRecorder::displayStats() const {
    std::cout << "\n========================================\n";
    std::cout << "      FINAL SIMULATION STATISTICS\n";
    std::cout << "========================================\n\n";

    // Set floating point precision for clean output.
    std::cout << std::fixed << std::setprecision(4);

    for (const auto& pair : statsMap) {

        if (pair.first == "NOT INITIALIZED") {continue;}

        const stats& stat = pair.second;

        std::cout << "--- " << pair.first << " ---\n";

        // Calculate averages, always checking for division by zero to prevent runtime errors.
        double averageFlightTime = (stat.flightsCompleted > 0) ? (stat.totalFlightTime / static_cast<double>(stat.flightsCompleted)) : 0.0;
        double averageDistance = (stat.flightsCompleted > 0) ? (stat.totalDistanceTravelled / static_cast<double>(stat.flightsCompleted)) : 0.0;
        double averageChargeTime = (stat.chargeSessionsCompleted > 0) ? (stat.timeSpentCharging / static_cast<double>(stat.chargeSessionsCompleted)) : 0.0;

        std::cout << "  Average Flight Time     : " << averageFlightTime / 3600.0 << " hours\n";
        std::cout << "  Average Distance Travelled: " << averageDistance << " miles\n";
        std::cout << "  Average Time Charging   : " << averageChargeTime / 3600.0 << " hours\n";
        std::cout << "  Total Number of Faults  : " << stat.totalNumFaults << "\n";
        std::cout << "  Total Passenger Miles   : " << stat.totalPassengerMiles << "\n\n";
    }
}
