# In tests/CMakeLists.txt

# --- Google Test Integration ---
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# For other projects, you might want to make this a little more robust
FetchContent_MakeAvailable(googletest)


# --- Define the Test Executable ---
add_executable(
        run_tests
        aircraftTest.cpp  # Add all your test files here
)

# Link your tests against your project library and Google Test
target_link_libraries(
        run_tests
        PRIVATE
        evotl_lib   # Your project's code
        GTest::gtest_main # Google Test framework
)

# Include Google Test headers
target_include_directories(run_tests PRIVATE ${gtest_SOURCE_DIR}/include)

# Add the test to ctest
include(GoogleTest)
gtest_add_tests(TARGET run_tests)