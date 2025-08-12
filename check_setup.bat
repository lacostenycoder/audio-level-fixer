@echo off
echo Testing Inno Setup Detection
echo ============================

echo Checking for Inno Setup...

set "INNO1=C:\Program Files (x86)\Inno Setup 6\ISCC.exe"
set "INNO2=C:\Program Files\Inno Setup 6\ISCC.exe"

if exist "%INNO1%" (
    echo Found: %INNO1%
) else (
    if exist "%INNO2%" (
        echo Found: %INNO2%
    ) else (
        echo Inno Setup 6 not found in standard locations
        echo Checking directories...
        dir "C:\Program Files\Inno*" /AD 2>nul
        dir "C:\Program Files (x86)\Inno*" /AD 2>nul
    )
)

echo.
echo Checking if application is built...
if exist "Build\AudioProcessor_artefacts\Release\Audio Processor.exe" (
    echo Application found
) else (
    echo Application NOT found - run build_windows.bat first
)

pause