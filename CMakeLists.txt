# Root CMakeLists.txt

project(evotl-sim)
cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 17)

# --- Main Project ---
include_directories(include)

# IMPORTANT: Restrict the glob to ONLY the src directory.
file(GLOB_RECURSE SOURCES "src/*.cpp")

# This now correctly creates a library with ONLY your application code.
add_library(evotl_lib ${SOURCES})
target_include_directories(evotl_lib PUBLIC include)

# This is fine.
add_executable(evotl-sim src/main.cpp)
target_link_libraries(evotl-sim evotl_lib)


# --- Testing Setup ---
enable_testing()
# This command passes control to the tests/CMakeLists.txt file.
add_subdirectory(tests)