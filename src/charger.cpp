//
// Created by William on 8/9/2025.
//
#include "../include/charger.h"
#include "../include/aircraft.h"

void charger::startCharging(aircraft *aircraft) {
    isBusy = true;
    currentAircraft = aircraft;
}

void charger::stopCharging() {
    isBusy = false;
    currentAircraft = nullptr;
}

