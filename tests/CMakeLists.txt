# In tests/CMakeLists.txt
cmake_policy(SET CMP0135 NEW)
# --- Google Test Integration ---
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)


add_executable(run_tests aircraftTest.cpp evotlSimTest.cpp)
set_target_properties(run_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)

target_compile_definitions(run_tests
        PRIVATE
        VEHICLE_CSV_PATH="${CMAKE_SOURCE_DIR}/vehicles.csv"
)

# --- The Fix ---
# Explicitly add the main project's include directory to the test target.
# CMAKE_SOURCE_DIR refers to the top-level project directory.
target_include_directories(run_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Link your tests against your project library and Google Test
target_link_libraries(
        run_tests
        PRIVATE
        evotl_lib           # Your project's code
        GTest::gtest_main   # Google Test framework
)

# Add the test to ctest
include(GoogleTest)
gtest_add_tests(TARGET run_tests)