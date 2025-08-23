@echo off
echo Testing Inno Setup Detection
echo ============================
echo Checking for Inno Setup...
set "INNO_PATH="
if exist "%ProgramFiles(x86)%\Inno Setup 6\ISCC.exe" set "INNO_PATH=%ProgramFiles(x86)%\Inno Setup 6\ISCC.exe"
if exist "%ProgramFiles%\Inno Setup 6\ISCC.exe"      set "INNO_PATH=%ProgramFiles%\Inno Setup 6\ISCC.exe"
if "%INNO_PATH%"=="" (
  echo Inno Setup not found. Skipping installer tests.
  exit /b 1
)
echo Inno Setup found at: %INNO_PATH%
exit /b 0
