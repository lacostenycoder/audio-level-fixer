@echo off
REM Audio Processor - Installer Testing Script
REM This script helps test the installer in various scenarios

echo Audio Processor - Installer Testing Script
echo ==========================================

set INSTALLER_PATH=Output\AudioProcessor-1.0.0-Setup.exe

REM Check if installer exists
if not exist "%INSTALLER_PATH%" (
    echo Error: Installer not found at %INSTALLER_PATH%
    echo Please build the installer first using build_installer.bat
    pause
    exit /b 1
)

echo Found installer: %INSTALLER_PATH%
echo.

:MENU
echo Testing Options:
echo 1. Normal Installation Test
echo 2. Silent Installation Test
echo 3. Very Silent Installation Test
echo 4. Custom Directory Installation Test
echo 5. Uninstall Test
echo 6. Check Installation Status
echo 7. Exit
echo.
set /p choice=Select option (1-7): 

if "%choice%"=="1" goto NORMAL_INSTALL
if "%choice%"=="2" goto SILENT_INSTALL
if "%choice%"=="3" goto VERY_SILENT_INSTALL
if "%choice%"=="4" goto CUSTOM_DIR_INSTALL
if "%choice%"=="5" goto UNINSTALL_TEST
if "%choice%"=="6" goto CHECK_STATUS
if "%choice%"=="7" goto EXIT
echo Invalid choice. Please try again.
goto MENU

:NORMAL_INSTALL
echo.
echo Running normal installation...
echo This will show the full installer UI.
echo.
pause
"%INSTALLER_PATH%"
goto MENU

:SILENT_INSTALL
echo.
echo Running silent installation...
echo This will install without user interaction but show progress.
echo.
pause
"%INSTALLER_PATH%" /SILENT
echo Installation completed.
goto MENU

:VERY_SILENT_INSTALL
echo.
echo Running very silent installation...
echo This will install with no UI at all.
echo.
pause
"%INSTALLER_PATH%" /VERYSILENT
echo Installation completed (if successful).
goto MENU

:CUSTOM_DIR_INSTALL
echo.
echo Running installation with custom directory...
set /p custom_dir=Enter installation directory (or press Enter for default): 
if "%custom_dir%"=="" (
    "%INSTALLER_PATH%" /SILENT
) else (
    "%INSTALLER_PATH%" /SILENT /DIR="%custom_dir%"
)
echo Installation completed.
goto MENU

:UNINSTALL_TEST
echo.
echo Testing uninstallation...
echo.

REM Check common uninstaller locations
set UNINSTALLER=
if exist "C:\Program Files\Audio Processor\unins000.exe" (
    set UNINSTALLER=C:\Program Files\Audio Processor\unins000.exe
) else if exist "C:\Program Files (x86)\Audio Processor\unins000.exe" (
    set UNINSTALLER=C:\Program Files (x86)\Audio Processor\unins000.exe
) else (
    echo Uninstaller not found. Audio Processor may not be installed.
    goto MENU
)

echo Found uninstaller: %UNINSTALLER%
echo.
echo Uninstall Options:
echo 1. Normal uninstall (with UI)
echo 2. Silent uninstall
echo 3. Cancel
set /p uninstall_choice=Select option (1-3): 

if "%uninstall_choice%"=="1" (
    "%UNINSTALLER%"
) else if "%uninstall_choice%"=="2" (
    "%UNINSTALLER%" /SILENT
    echo Uninstallation completed.
) else (
    echo Uninstall cancelled.
)
goto MENU

:CHECK_STATUS
echo.
echo Checking installation status...
echo.

REM Check if application is installed
if exist "C:\Program Files\Audio Processor\Audio Processor.exe" (
    echo ✓ Audio Processor is installed in C:\Program Files\Audio Processor\
    
    REM Get file version
    for /f "tokens=*" %%i in ('powershell -command "(Get-ItemProperty 'C:\Program Files\Audio Processor\Audio Processor.exe').VersionInfo.FileVersion"') do set FILE_VERSION=%%i
    echo   Version: %FILE_VERSION%
    
    REM Check file size
    for %%i in ("C:\Program Files\Audio Processor\Audio Processor.exe") do echo   Size: %%~zi bytes
    
) else if exist "C:\Program Files (x86)\Audio Processor\Audio Processor.exe" (
    echo ✓ Audio Processor is installed in C:\Program Files (x86)\Audio Processor\
) else (
    echo ✗ Audio Processor is not installed
)

REM Check registry entries
echo.
echo Checking registry entries...
reg query "HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Audio Processor" >nul 2>&1
if %ERRORLEVEL%==0 (
    echo ✓ Uninstall registry entry found
) else (
    echo ✗ Uninstall registry entry not found
)

reg query "HKLM\SOFTWARE\Audio Processor Development Team\Audio Processor" >nul 2>&1
if %ERRORLEVEL%==0 (
    echo ✓ Application registry entry found
) else (
    echo ✗ Application registry entry not found
)

REM Check Start Menu shortcuts
if exist "%PROGRAMDATA%\Microsoft\Windows\Start Menu\Programs\Audio Processor.lnk" (
    echo ✓ Start Menu shortcut found
) else (
    echo ✗ Start Menu shortcut not found
)

REM Check Desktop shortcut
if exist "%PUBLIC%\Desktop\Audio Processor.lnk" (
    echo ✓ Desktop shortcut found
) else (
    echo ✗ Desktop shortcut not found (may be optional)
)

echo.
goto MENU

:EXIT
echo.
echo Testing completed.
echo.
echo Additional manual tests to perform:
echo 1. Launch Audio Processor and verify it works
echo 2. Check virtual audio device in Windows Sound settings
echo 3. Test with recording software (OBS, Audacity, etc.)
echo 4. Verify uninstaller removes all components
echo 5. Test on different Windows versions and user accounts
echo 6. Check with antivirus software for false positives
echo.
pause
exit /b 0

