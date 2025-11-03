@echo off

set "BUILD_TYPE=Release"

if "%1" neq "" (
    if /i "%1"=="Debug" (
        set "BUILD_TYPE=Debug"
    ) else if /i "%1"=="Release" (
        set "BUILD_TYPE=Release"
    ) else (
        echo ERROR: Invalid argument. Use "Debug" or "Release".
        exit /b 1
    )
)

git --version >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo ERROR: Git is not installed or not found in PATH.
    echo Please install Git and try again.
    exit /b 1
)

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

for /f "delims=" %%i in ('.\vcpkg.exe fetch cmake') do set "VCPKG_CMAKE=%%i"

set "VULKAN_SDK=%cd%\installed\x64-windows"
cd ..
if not exist "build" mkdir build

"%VCPKG_CMAKE%" -S . -B build -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -DCMAKE_TOOLCHAIN_FILE=%cd%\vcpkg\scripts\buildsystems\vcpkg.cmake
"%VCPKG_CMAKE%" --build build --config %BUILD_TYPE%
