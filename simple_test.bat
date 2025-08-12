@echo off
echo Simple Test
echo ===========

echo Checking Inno Setup locations...
if exist "C:\Program Files\Inno Setup 6\ISCC.exe" echo Found in Program Files
if exist "C:\Program Files (x86)\Inno Setup 6\ISCC.exe" echo Found in Program Files x86

echo.
echo Done
pause