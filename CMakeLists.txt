# Root CMakeLists.txt

project(evotl-sim)
cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 17)

# This is more robust than using file(GLOB...).
add_library(evotl_lib
        src/aircraft.cpp
        src/charger.cpp
        src/evotlSim.cpp
        src/statisticsRecorder.cpp
)

target_include_directories(evotl_lib PUBLIC include)

# This is fine.
add_executable(evotl-sim src/main.cpp)
target_link_libraries(evotl-sim evotl_lib)


# --- Testing Setup ---
enable_testing()
# This command passes control to the tests/CMakeLists.txt file.
add_subdirectory(tests)