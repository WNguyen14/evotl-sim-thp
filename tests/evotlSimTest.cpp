//
// Created by William on 8/10/2025.
//
#include "gtest/gtest.h"
#include "evotlSim.h"
#include <memory>

#ifndef VEHICLE_CSV_PATH
#error "VEHICLE_CSV_PATH is not defined. Check your tests/CMakeLists.txt"
#endif

class testableSimulation : public evotlSim {
public:
    // Constructor that passes arguments to the base evotlSim constructor
    testableSimulation(int numVehicles, int numChargers, int hours, const std::string& vehicleCsvPath)
        : evotlSim(numVehicles, numChargers, hours, vehicleCsvPath) {}

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
    std::vector<std::unique_ptr<aircraft>>& getFleet() { return fleet; }
    std::vector<aircraftType>& getAircraftTypes() { return aircraftTypes; }
    const std::map<std::string, stats>& getFinalStats() const {
            return statisticsRecorder.getStatsMap();
        }
};

class evotlSimTest : public ::testing::Test {
protected:
    testableSimulation* sim;

    void SetUp() override {
        // Create a sim with specific, non-random conditions for testing
        sim = new testableSimulation(1, 1, 1, VEHICLE_CSV_PATH); // 1 vehicle, 1 charger
    }

    void TearDown() override {
        delete sim;
    }
};

TEST_F(evotlSimTest, QueueLogic_AircraftIsForcedToWaitWhenNoChargersAvailable) {
    // Arrange: Create a simulation with 1 aircraft but 0 chargers.
    delete sim; // Delete the default sim from SetUp
    sim = new testableSimulation(1, 0, 1, VEHICLE_CSV_PATH); // 1 vehicle, ZERO chargers

    sim->getFleet().clear();
    sim->getFleet().push_back(std::make_unique<aircraft>(sim->getAircraftTypes()[0]));

    aircraft* theAircraft = sim->getFleet()[0].get();

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
    sim->getFleet().push_back(std::make_unique<aircraft>(sim->getAircraftTypes()[0]));

    aircraft* theAircraft = sim->getFleet()[0].get();

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
    delete sim;
    sim = new testableSimulation(1, 1, 3, VEHICLE_CSV_PATH);

    // Manually create a fleet with only one "Bravo" aircraft to ensure determinism
    sim->getFleet().clear();
    sim->getFleet().push_back(std::make_unique<aircraft>(sim->getAircraftTypes()[1]));

    // Act
    sim->test_runSimulation();

    // Assert
    const auto& statsMap = sim->getFinalStats();
    ASSERT_TRUE(statsMap.count("Bravo"));
    const stats& bravoStats = statsMap.at("Bravo");

    const double tolerance = 0.1;

    // Verify flight counts and times
    ASSERT_EQ(bravoStats.flightsCompleted, 4);
    EXPECT_NEAR(bravoStats.totalFlightTime, 8640.0, tolerance);
    double expectedAvgFlightTime = 8640.0 / 4.0;
    EXPECT_NEAR(bravoStats.totalFlightTime / bravoStats.flightsCompleted, expectedAvgFlightTime, tolerance);

    // Verify distances
    EXPECT_NEAR(bravoStats.totalDistanceTravelled, 240.0, tolerance);
    double expectedAvgDistance = 240.0 / 4.0;
    EXPECT_NEAR(bravoStats.totalDistanceTravelled / bravoStats.flightsCompleted, expectedAvgDistance, tolerance);

    // Verify charge counts and times
    ASSERT_EQ(bravoStats.chargeSessionsCompleted, 3);
    EXPECT_NEAR(bravoStats.timeSpentCharging, 2160.0, tolerance);
    double expectedAvgChargeTime = 2160.0 / 3.0;
    EXPECT_NEAR(bravoStats.timeSpentCharging / bravoStats.chargeSessionsCompleted, expectedAvgChargeTime, tolerance);

    // Verify passenger miles
    EXPECT_NEAR(bravoStats.totalPassengerMiles, 1200.0, tolerance);
}

TEST_F(evotlSimTest, MultiAircraft_ResourceContention_EndToEnd_StatsVerification) {
    // --- ARRANGE ---
    delete sim;
    // Use our deterministic simulation class with 2 vehicles, 1 charger, for a 3-hour run.
    sim = new testableSimulation(2, 1, 3, VEHICLE_CSV_PATH);

    // Manually create a fleet with one Bravo and one Delta aircraft for a predictable scenario.
    sim->getFleet().clear();
    const auto& types = sim->getAircraftTypes();
    aircraftType bravoType = types[1]; // Bravo is at index 1
    aircraftType deltaType = types[3]; // Delta is at index 3
    sim->getFleet().push_back(std::make_unique<aircraft>(bravoType));
    sim->getFleet().push_back(std::make_unique<aircraft>(deltaType));

    // --- ACT ---
    sim->test_runSimulation();

    // --- ASSERT ---
    // Retrieve the final statistics and verify them against our corrected "golden" values.
    const auto& statsMap = sim->getFinalStats();
    const double tolerance = 1; // Tolerance for floating point comparisons

    // --- Verify Bravo Stats ---
    ASSERT_TRUE(statsMap.count("Bravo"));
    const stats& bravoStats = statsMap.at("Bravo");
    
    EXPECT_EQ(bravoStats.flightsCompleted, 4);
    EXPECT_EQ(bravoStats.chargeSessionsCompleted, 3);
    EXPECT_NEAR(bravoStats.totalFlightTime, 8640.0, tolerance);
    EXPECT_NEAR(bravoStats.timeSpentCharging, 2160.0, tolerance);
    EXPECT_NEAR(bravoStats.totalDistanceTravelled, 240.0, tolerance);
    EXPECT_NEAR(bravoStats.totalPassengerMiles, 1200.0, tolerance);

    // --- Verify Delta Stats ---
    ASSERT_TRUE(statsMap.count("Delta"));
    const stats& deltaStats = statsMap.at("Delta");

    EXPECT_EQ(deltaStats.flightsCompleted, 2);
    EXPECT_EQ(deltaStats.chargeSessionsCompleted, 1);
    EXPECT_NEAR(deltaStats.totalFlightTime, 8328.0, tolerance);
    EXPECT_NEAR(deltaStats.timeSpentCharging, 2232.0, tolerance);
    EXPECT_NEAR(deltaStats.totalDistanceTravelled, 208.2, tolerance);
    EXPECT_NEAR(deltaStats.totalPassengerMiles, 416.4, tolerance);
}