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

cd vcpkg
call .\bootstrap-vcpkg.bat
.\vcpkg integrate install
.\vcpkg install glfw3 eigen3 glslang vulkan
set "VULKAN_SDK=%cd%\installed\x64-windows"
cd ..
if not exist "build" mkdir build
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=%cd%\vcpkg\scripts\buildsystems\vcpkg.cmake
cmake --build build
