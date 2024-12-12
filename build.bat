@echo off

REM Check if Git is installed
echo Checking for Git...
git --version >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo ERROR: Git is not installed or not found in PATH.
    echo Please install Git and try again.
    exit /b 1
)

REM Clone the vcpkg repository
echo Cloning vcpkg repository...
git clone https://github.com/microsoft/vcpkg.git

REM Bootstrap vcpkg
echo Bootstrapping vcpkg...
cd vcpkg
.\bootstrap-vcpkg.bat

REM Integrate vcpkg with the environment
echo Integrating vcpkg with the environment...
.\vcpkg integrate install

REM Install required packages
echo Installing dependencies via vcpkg...
.\vcpkg install glfw3 eigen3 glslang vulkan

REM Set Vulkan SDK environment variable
echo Setting VULKAN_SDK environment variable...
set VULKAN_SDK=%cd%\vcpkg\installed\x64-windows

REM Go back to the project directory and create a build directory
cd ..
mkdir build

REM Run CMake with the vcpkg toolchain file
echo Running CMake to configure the project...
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=%cd%\vcpkg\scripts\buildsystems\vcpkg.cmake

REM Build the project
echo Building the project...
cmake --build build

echo Build complete!
pause
