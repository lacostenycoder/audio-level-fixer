@echo off
REM Audio Processor - Windows Installer Build Script
REM This script builds the Windows installer using Inno Setup

echo Audio Processor - Windows Installer Build Script
echo ================================================

REM Check for Inno Setup
set INNO_SETUP_PATH=
set "INNO1=C:\Program Files (x86)\Inno Setup 6\ISCC.exe"
set "INNO2=C:\Program Files\Inno Setup 6\ISCC.exe"

if exist "%INNO1%" (
    set "INNO_SETUP_PATH=%INNO1%"
) else (
    if exist "%INNO2%" (
        set "INNO_SETUP_PATH=%INNO2%"
    ) else (
        echo Error: Inno Setup 6 not found.
        echo Please install Inno Setup 6 from: https://jrsoftware.org/isinfo.php
        echo.
        echo Expected locations:
        echo   C:\Program Files (x86)\Inno Setup 6\ISCC.exe
        echo   C:\Program Files\Inno Setup 6\ISCC.exe
        pause
        exit /b 1
    )
)

echo Found Inno Setup: %INNO_SETUP_PATH%

REM Check if application is built
if not exist "Build\AudioProcessor_artefacts\Release\Audio Processor.exe" (
    echo Error: Audio Processor application not found.
    echo Please build the application first using build_windows.bat
    echo Expected location: Build\AudioProcessor_artefacts\Release\Audio Processor.exe
    pause
    exit /b 1
)

echo ✓ Application executable found

REM Create necessary directories
echo Setting up installer environment...
if not exist "Output" mkdir Output
if not exist "Redist" mkdir Redist

REM Check for Visual C++ Redistributable
if not exist "Redist\VC_redist.x64.exe" (
    echo Warning: Visual C++ Redistributable not found at Redist\VC_redist.x64.exe
    echo You can download it from:
    echo https://aka.ms/vs/17/release/vc_redist.x64.exe
    echo.
    echo The installer will work without it, but users may need to install it separately.
    echo.
)

REM Create default resources if they don't exist
echo Checking for resources...
if not exist "..\Resources" mkdir "..\Resources"

REM Create default icon if it doesn't exist
if not exist "..\Resources\icon.ico" (
    echo Creating default application icon...
    REM This would normally copy a proper icon file
    echo Warning: Default icon not found. Using system default.
)

REM Create default license if it doesn't exist
if not exist "..\LICENSE.txt" (
    echo Creating default license file...
    echo Audio Processor License > "..\LICENSE.txt"
    echo. >> "..\LICENSE.txt"
    echo Copyright (C) 2025 Audio Processor Development Team >> "..\LICENSE.txt"
    echo. >> "..\LICENSE.txt"
    echo This software is provided "as is" without warranty of any kind. >> "..\LICENSE.txt"
)

REM Create default presets directory
if not exist "..\Presets" mkdir "..\Presets"
if not exist "..\Presets\Default.preset" (
    echo Creating default preset files...
    echo [Default Preset] > "..\Presets\Default.preset"
    echo InputGain=0.0 >> "..\Presets\Default.preset"
    echo CompressorThreshold=-20.0 >> "..\Presets\Default.preset"
    echo CompressorRatio=4.0 >> "..\Presets\Default.preset"
    echo LimiterCeiling=-0.3 >> "..\Presets\Default.preset"
)

echo ✓ Environment setup completed

REM Build the installer
echo Building installer with Inno Setup...
echo.
"%INNO_SETUP_PATH%" "AudioProcessor.iss"

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ✅ Installer built successfully!
    echo.
    echo Installer location: %CD%\Output\
    dir Output\*.exe
    echo.
    echo The installer is ready for distribution.
    echo.
    echo Next steps:
    echo 1. Test the installer on a clean Windows system
    echo 2. Sign the installer with a code signing certificate
    echo 3. Test with Windows Defender and other antivirus software
    echo 4. Distribute to users
    echo.
) else (
    echo.
    echo ❌ Installer build failed!
    echo Please check the error messages above.
    echo.
)

pause

