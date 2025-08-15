# Root CMakeLists.txt
cmake_policy(SET CMP0135 NEW)
cmake_minimum_required(VERSION 3.16)
project(evotl-sim)
set(CMAKE_CXX_STANDARD 17)
add_library(evotl_lib
        src/aircraft.cpp
        src/charger.cpp
        src/evotlSim.cpp
        src/statisticsRecorder.cpp
)

target_include_directories(evotl_lib PUBLIC include)

add_executable(evotl-sim src/main.cpp)
target_link_libraries(evotl-sim evotl_lib)

enable_testing()
add_subdirectory(tests)
