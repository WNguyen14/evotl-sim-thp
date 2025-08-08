project(evotl-sim)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)

file(GLOB SOURCES "src/*.cpp")

add_executable(evotl-sim ${SOURCES}
        include/aircraft.h
        include/evotlSim.cpp
        include/aircraftType.hpp)