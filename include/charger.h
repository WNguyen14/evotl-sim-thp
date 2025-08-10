//
// Created by William on 8/9/2025.
//

#ifndef EVOTL_SIM_CHARGER_H
#define EVOTL_SIM_CHARGER_H
#include "aircraft.h"


class charger {
public:
    charger() = default;
    ~charger() = default;

    void startCharging(aircraft* aircraft);
    void stopCharging();
    bool getIsBusy() const {return isBusy;}
    aircraft* getCurrentAircraft() const {return currentAircraft;};

private:
    bool isBusy{false};
    aircraft* currentAircraft{nullptr};

};


#endif //EVOTL_SIM_CHARGER_H