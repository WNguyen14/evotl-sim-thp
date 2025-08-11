//
// Created by William on 8/10/2025.
//
#include "gtest/gtest.h"
#include "evotlSim.h" // Your main simulation header


#ifndef VEHICLE_CSV_PATH
#error "VEHICLE_CSV_PATH is not defined. Check your tests/CMakeLists.txt"
#endif

class testableSimulation : public evotlSim {
public:
    // Constructor that passes arguments to the base evotlSim constructor
    testableSimulation(int numVehicles, int numChargers, int hours)
        : evotlSim(numVehicles, numChargers, hours) {}

    void setup_initTypesFromCsv(const std::string& fileName) {
        initTypesFromCsv(fileName);
    }


    void test_handleStep(double timeElapsed) {
        handleStep(timeElapsed);
    }

    // Public backdoors to check the internal state of the simulation
    std::queue<aircraft*>& getAircraftQueue() { return aircraftQueue; }
    std::vector<charger>& getChargers() { return chargers; }
    std::vector<aircraft>& getFleet() { return fleet; }
    std::vector<aircraftType>& getAircraftTypes() { return aircraftTypes; }

};

class evotlSimTest : public ::testing::Test {
protected:
    testableSimulation* sim;

    void SetUp() override {
        // Create a sim with specific, non-random conditions for testing
        sim = new testableSimulation(1, 1, 1); // 1 vehicle, 1 charger
        // We manually initialize to control the test environment
        sim->setup_initTypesFromCsv(VEHICLE_CSV_PATH);
        // We'll manually create the fleet to avoid randomness
    }

    void TearDown() override {
        delete sim;
    }
};

TEST_F(evotlSimTest, QueueLogic_AircraftIsForcedToWaitWhenNoChargersAvailable) {
    // Arrange: Create a simulation with 1 aircraft but 0 chargers.
    delete sim; // Delete the default sim from SetUp
    sim = new testableSimulation(1, 0, 1); // 1 vehicle, ZERO chargers
    sim->setup_initTypesFromCsv(VEHICLE_CSV_PATH);

    sim->getFleet().clear();
    sim->getFleet().emplace_back(sim->getAircraftTypes()[0]);

    aircraft* theAircraft = &sim->getFleet()[0];

    // Set charge low enough to be depleted in one step
    theAircraft->setCharge(0.1);

    // Act
    sim->test_handleStep(10.0);

    // Assert: Now, the queue MUST contain the aircraft.
    ASSERT_EQ(sim->getAircraftQueue().size(), 1);
    EXPECT_EQ(sim->getAircraftQueue().front(), theAircraft);
    EXPECT_EQ(theAircraft->getAircraftState(), aircraft::WAITING);
}

TEST_F(evotlSimTest, FullCycle_DepletesAndImmediatelyChargesWithAvailableCharger) {
    // Arrange (This uses the default SetUp: 1 vehicle, 1 charger)
    sim->getFleet().clear();
    sim->getFleet().emplace_back(sim->getAircraftTypes()[0]);

    aircraft* theAircraft = &sim->getFleet()[0];

    theAircraft->setCharge(0.1);

    // Act
    sim->test_handleStep(10.0);

    // Assert: The final state is CHARGING, not WAITING.
    // The queue should be empty because the aircraft was immediately picked up.
    EXPECT_TRUE(sim->getAircraftQueue().empty());
    EXPECT_EQ(theAircraft->getAircraftState(), aircraft::CHARGING);

    // We can also assert that the charger is now busy with our aircraft.
    EXPECT_TRUE(sim->getChargers()[0].getIsBusy());
    EXPECT_EQ(sim->getChargers()[0].getCurrentAircraft(), theAircraft);
}