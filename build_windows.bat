@echo off
REM Audio Processor - Windows Build Script
REM This script builds the Audio Processor application for Windows

echo Audio Processor - Windows Build Script
echo ======================================

REM Check for Visual Studio or Build Tools
where cl >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo Error: Visual Studio or Build Tools for Visual Studio is required.
    echo Please install Visual Studio 2019 or later with C++ development tools.
    echo Or install Build Tools for Visual Studio from:
    echo https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2019
    pause
    exit /b 1
)

REM Check for CMake
where cmake >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo Error: CMake is required but not found in PATH.
    echo Please install CMake from: https://cmake.org/download/
    pause
    exit /b 1
)

echo ✓ Dependencies check completed

REM Create build directory
echo Setting up build environment...
set BUILD_DIR=Build
if exist "%BUILD_DIR%" (
    echo Cleaning existing build directory...
    rmdir /s /q "%BUILD_DIR%"
)

mkdir "%BUILD_DIR%"
cd "%BUILD_DIR%"

echo ✓ Build directory created

REM Configure with CMake for Visual Studio
echo Configuring build with CMake...
cmake .. -G "Visual Studio 16 2019" -A x64 -DCMAKE_BUILD_TYPE=Release

if %ERRORLEVEL% NEQ 0 (
    echo Error: CMake configuration failed
    pause
    exit /b 1
)

echo ✓ CMake configuration completed

REM Build the application
echo Building Audio Processor...
cmake --build . --config Release --parallel

if %ERRORLEVEL% NEQ 0 (
    echo Error: Build failed
    pause
    exit /b 1
)

echo ✓ Build completed successfully

REM Check if executable was created
set EXECUTABLE_PATH=AudioProcessor_artefacts\Release\Audio Processor.exe
if exist "%EXECUTABLE_PATH%" (
    echo ✓ Executable created: %EXECUTABLE_PATH%
) else (
    echo Error: Executable not found at expected location
    pause
    exit /b 1
)

echo.
echo 🎉 Build completed successfully!
echo.
echo Application location: %CD%\%EXECUTABLE_PATH%
echo.
echo To run the application:
echo   cd %CD%
echo   "%EXECUTABLE_PATH%"
echo.
echo Note: The application includes WASAPI virtual audio device support.
echo Make sure Windows audio service is running for full functionality.
echo.
pause

