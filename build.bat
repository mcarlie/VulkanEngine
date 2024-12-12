@echo off

REM Check if Git is installed
echo Checking for Git...
git --version >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo ERROR: Git is not installed or not found in PATH.
    echo Please install Git and try again.
    exit /b 1
)

REM Check if vcpkg directory exists
if not exist "vcpkg" (
    echo Cloning the vcpkg repository...
    git clone https://github.com/microsoft/vcpkg.git
    if %ERRORLEVEL% neq 0 (
        echo ERROR: Failed to clone vcpkg repository.
        exit /b 1
    )
) else (
    echo vcpkg directory already exists. Skipping clone.
)

REM Bootstrap vcpkg
echo Bootstrapping vcpkg...
cd vcpkg
call .\bootstrap-vcpkg.bat
if %ERRORLEVEL% neq 0 (
    echo ERROR: Failed to bootstrap vcpkg.
    exit /b 1
)

REM Integrate vcpkg with the environment
echo Integrating vcpkg with the environment...
.\vcpkg integrate install
if %ERRORLEVEL% neq 0 (
    echo ERROR: vcpkg integration failed.
    exit /b 1
)

REM Install required packages
echo Installing dependencies via vcpkg...
.\vcpkg install glfw3 eigen3 glslang vulkan
if %ERRORLEVEL% neq 0 (
    echo ERROR: Failed to install required dependencies.
    exit /b 1
)

REM Set Vulkan SDK environment variable
echo Setting VULKAN_SDK environment variable...
set "VULKAN_SDK=%cd%\installed\x64-windows"

REM Return to the project directory
cd ..
if not exist "build" mkdir build

REM Run CMake with the vcpkg toolchain file
echo Running CMake to configure the project...
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=%cd%\vcpkg\scripts\buildsystems\vcpkg.cmake
if %ERRORLEVEL% neq 0 (
    echo ERROR: CMake configuration failed.
    exit /b 1
)

REM Build the project
echo Building the project...
cmake --build build
if %ERRORLEVEL% neq 0 (
    echo ERROR: Build process failed.
    exit /b 1
)

echo Build complete!
pause
