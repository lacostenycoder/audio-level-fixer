@echo off
REM Audio Processor - Code Signing Script
REM This script signs the installer with a digital certificate

echo Audio Processor - Code Signing Script
echo =====================================

REM Configuration - Update these paths for your certificate
set CERT_FILE=certificate.pfx
set CERT_PASSWORD=your_password_here
set TIMESTAMP_URL=http://timestamp.digicert.com
set INSTALLER_PATH=Output\AudioProcessor-1.0.0-Setup.exe

REM Check for signtool
set SIGNTOOL_PATH=
if exist "C:\Program Files (x86)\Windows Kits\10\bin\x64\signtool.exe" (
    set SIGNTOOL_PATH=C:\Program Files (x86)\Windows Kits\10\bin\x64\signtool.exe
) else if exist "C:\Program Files\Microsoft SDKs\Windows\v7.1\Bin\signtool.exe" (
    set SIGNTOOL_PATH=C:\Program Files\Microsoft SDKs\Windows\v7.1\Bin\signtool.exe
) else (
    echo Error: signtool.exe not found.
    echo Please install Windows SDK or Visual Studio with Windows development tools.
    echo.
    echo Expected locations:
    echo   C:\Program Files (x86)\Windows Kits\10\bin\x64\signtool.exe
    echo   C:\Program Files\Microsoft SDKs\Windows\v7.1\Bin\signtool.exe
    pause
    exit /b 1
)

echo Found signtool: %SIGNTOOL_PATH%

REM Check if installer exists
if not exist "%INSTALLER_PATH%" (
    echo Error: Installer not found at %INSTALLER_PATH%
    echo Please build the installer first using build_installer.bat
    pause
    exit /b 1
)

echo Found installer: %INSTALLER_PATH%

REM Check if certificate exists
if not exist "%CERT_FILE%" (
    echo Error: Certificate file not found at %CERT_FILE%
    echo.
    echo Please obtain a code signing certificate and update the configuration in this script:
    echo 1. Purchase a code signing certificate from a trusted CA
    echo 2. Save the certificate as %CERT_FILE%
    echo 3. Update CERT_PASSWORD in this script
    echo.
    echo Recommended certificate authorities:
    echo - DigiCert
    echo - Sectigo (formerly Comodo)
    echo - GlobalSign
    echo - Entrust
    echo.
    pause
    exit /b 1
)

echo Found certificate: %CERT_FILE%

REM Prompt for password if not set
if "%CERT_PASSWORD%"=="your_password_here" (
    echo.
    set /p CERT_PASSWORD=Enter certificate password: 
)

echo.
echo Signing installer...
echo.

REM Sign the installer
"%SIGNTOOL_PATH%" sign /f "%CERT_FILE%" /p "%CERT_PASSWORD%" /t "%TIMESTAMP_URL%" /d "Audio Processor" /du "https://github.com/audioprocessor/audioprocessor" "%INSTALLER_PATH%"

if %ERRORLEVEL%==0 (
    echo.
    echo ✅ Installer signed successfully!
    echo.
    
    REM Verify the signature
    echo Verifying signature...
    "%SIGNTOOL_PATH%" verify /pa "%INSTALLER_PATH%"
    
    if %ERRORLEVEL%==0 (
        echo ✅ Signature verification successful!
        echo.
        echo The installer is now ready for distribution.
        echo.
        echo Signed installer: %INSTALLER_PATH%
        
        REM Show file information
        echo.
        echo File information:
        dir "%INSTALLER_PATH%"
        
        echo.
        echo Certificate information:
        "%SIGNTOOL_PATH%" verify /v "%INSTALLER_PATH%"
        
    ) else (
        echo ❌ Signature verification failed!
        echo Please check the certificate and try again.
    )
    
) else (
    echo ❌ Signing failed!
    echo.
    echo Common issues:
    echo 1. Incorrect certificate password
    echo 2. Certificate has expired
    echo 3. Certificate is not valid for code signing
    echo 4. Network issues with timestamp server
    echo.
    echo Please check the error message above and try again.
)

echo.
pause

