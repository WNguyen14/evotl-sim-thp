# In tests/CMakeLists.txt
cmake_policy(SET CMP0135 NEW)

# --- Google Test Integration ---
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

# --- Define the executable ---
# This just creates the program; no testing "magic" attached.
add_executable(run_tests aircraftTest.cpp evotlSimTest.cpp)

target_compile_definitions(run_tests
        PRIVATE
        VEHICLE_CSV_PATH="${CMAKE_SOURCE_DIR}/vehicles.csv"
)

target_include_directories(run_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- Link the libraries ---
target_link_libraries(
        run_tests
        PRIVATE
        evotl_lib
        GTest::gtest_main
)