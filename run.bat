@echo off
echo --- Starting clean build and run process for Windows ---

:: Step 1: Clean the workspace
echo [1/4] Deleting old build directory...
if exist build rmdir /s /q build

:: Step 2: Configure and build
echo [2/4] Configuring with CMake and building with MSVC...
mkdir build
cd build
cmake ..
cmake --build . --config Debug

:: Check if the build was successful before proceeding
if %errorlevel% neq 0 (
    echo BUILD FAILED. Aborting.
    exit /b %errorlevel%
)

:: Step 3: Run the simulation
echo.
echo [3/4] Running the main eVTOL simulation...
.\Debug\evotl-sim.exe ../vehicles.csv

:: Step 4: Run the tests
echo.
echo [4/4] Running the unit tests...
.\tests\Debug\run_tests.exe

echo.
echo --- Clean build and run process finished ---