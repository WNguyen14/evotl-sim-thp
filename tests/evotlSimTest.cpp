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

    void test_runSimulation() {
        runSimulation();
    }


    // Public backdoors to check the internal state of the simulation
    std::queue<aircraft*>& getAircraftQueue() { return aircraftQueue; }
    std::vector<charger>& getChargers() { return chargers; }
    std::vector<aircraft>& getFleet() { return fleet; }
    std::vector<aircraftType>& getAircraftTypes() { return aircraftTypes; }
    const std::map<std::string, stats>& getFinalStats() const {
            return statisticsRecorder.getStatsMap(); // Assumes a getter in statisticsRecorder
        }
};

class evotlSimTest : public ::testing::Test {
protected:
    testableSimulation* sim;

    void SetUp() override {
        // Create a sim with specific, non-random conditions for testing
        sim = new testableSimulation(1, 1, 1); // 1 vehicle, 1 charger
        // We manually initialize to control the test environment
        sim->initSimulation();
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
    sim->initSimulation();

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

TEST_F(evotlSimTest, Statistics_VerifyEndToEndResultsForDeterministicScenario) {
    // Arrange
    // Overwrite the default sim with our specific "laboratory" conditions
    delete sim;
    sim = new testableSimulation(1, 1, 3); // 1 vehicle, 1 charger, 3 hours
    sim->initSimulation();

    // Manually create a fleet with only one "Bravo" aircraft to ensure determinism
    sim->getFleet().clear();
    // Index 1 corresponds to Bravo Company in vehicles.csv
    sim->getFleet().emplace_back(sim->getAircraftTypes()[1]);

    // Act
    // Run the entire simulation from start to finish. This tests the whole system.
    sim->test_runSimulation(); // We need to make runSimulation() public/protected

    // Assert
    const auto& statsMap = sim->getFinalStats();
    // Check that our Bravo Company was actually simulated
    ASSERT_TRUE(statsMap.count("Bravo"));
    const stats& bravoStats = statsMap.at("Bravo");

    // --- Verify the results against our paper calculations ---

    // Verify flight counts and times
    ASSERT_EQ(bravoStats.flightsCompleted, 4);
    EXPECT_DOUBLE_EQ(bravoStats.totalFlightTime, 8640.0);
    double expectedAvgFlightTime = 8640.0 / 4.0;
    EXPECT_DOUBLE_EQ(bravoStats.totalFlightTime / bravoStats.flightsCompleted, expectedAvgFlightTime);

    // Verify distances
    EXPECT_NEAR(bravoStats.totalDistanceTravelled, 240.0, 1e-5); // Use NEAR for float precision
    double expectedAvgDistance = 240.0 / 4.0;
    EXPECT_NEAR(bravoStats.totalDistanceTravelled / bravoStats.flightsCompleted, expectedAvgDistance, 1e-5);

    // Verify charge counts and times
    ASSERT_EQ(bravoStats.chargeSessionsCompleted, 3);
    EXPECT_DOUBLE_EQ(bravoStats.timeSpentCharging, 2160.0);
    double expectedAvgChargeTime = 2160.0 / 3.0;
    EXPECT_DOUBLE_EQ(bravoStats.timeSpentCharging / bravoStats.chargeSessionsCompleted, expectedAvgChargeTime);

    // Verify passenger miles
    EXPECT_NEAR(bravoStats.totalPassengerMiles, 1200.0, 1e-5);
}