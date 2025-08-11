//
// Created by William on 8/10/2025.
//

// In tests/aircraft_test.cpp
#include "gtest/gtest.h"
#include "aircraft.h"
#include "aircraftType.hpp"

// A Test Fixture allows us to reuse the same setup for multiple tests.
class aircraftTest : public ::testing::Test {
protected:
    void SetUp() override {
        // This runs before each test
        // Arrange: Create a predictable aircraft type and the aircraft itself.
        testType = aircraftType("Test Bravo", 100.0, 100.0, 0.2, 1.5, 5, 0.1);
        testAircraft = new aircraft(testType);
    }

    void TearDown() override {
        // Clean up
        delete testAircraft;
    }

    aircraftType testType;
    aircraft* testAircraft = nullptr;
};

// TEST_F uses the fixture we defined above.
TEST_F(aircraftTest, UpdateFlight_SufficientCharge) {
    // Act: Simulate flying for 36 seconds (0.01 hours).
    double timeFlown = testAircraft->updateFlight(36.0);

    // Assert: Check if the calculations are correct.
    // Distance = speed (100 mph) * time (0.01 h) = 1 mile
    // Energy Used = distance (1 mile) * usage (1.5 kWh/mile) = 1.5 kWh
    // Remaining Charge = 100 kWh - 1.5 kWh = 98.5 kWh
    ASSERT_DOUBLE_EQ(timeFlown, 36.0);
    ASSERT_DOUBLE_EQ(testAircraft->getCurrentFlightDistance(), 1.0);
    ASSERT_DOUBLE_EQ(testAircraft->getCurrentCharge(), 98.5);
    ASSERT_EQ(testAircraft->getAircraftState(), aircraft::FLYING);
}

// In tests/aircraftTest.cpp, inside the aircraftTest fixture

// Use EXPECT_NEAR for floating-point comparisons to avoid precision issues.
TEST_F(aircraftTest, UpdateFlight_PartialFlightOnDepletion) {
    // Arrange: Set the charge to a low value where it will deplete mid-step.
    // Based on our calculation, 1.0 kWh should last for 24 seconds.
    testAircraft->setCharge(1.0); // Manually setting for the test

    // Act: Simulate for 30 seconds, which is longer than the battery will last.
    double timeFlown = testAircraft->updateFlight(30.0);

    // Assert: Check that the aircraft only flew for 24 seconds and is now WAITING.
    EXPECT_NEAR(timeFlown, 24.0, 1e-5);
    EXPECT_NEAR(testAircraft->getCurrentCharge(), 0.0, 1e-5);
    EXPECT_EQ(testAircraft->getAircraftState(), aircraft::WAITING);

    // Distance = speed (100 mph) * time (24 sec / 3600 sec/hr) = 0.666... miles
    EXPECT_NEAR(testAircraft->getCurrentFlightDistance(), 100.0 * (24.0 / 3600.0), 1e-5);
}