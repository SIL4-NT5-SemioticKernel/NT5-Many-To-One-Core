@echo off
cd src
if not exist build mkdir build
cd build

:: Generate build files and build the project
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .

:: Move the binary to the bin directory
if exist "..\bin" (
    copy debug\NTSIL.exe ..\..\bin\NTSIL.exe
) else (
    mkdir ..\..\bin
    copy debug\NTSIL.exe ..\..\bin\NTSIL.exe
)

echo Build completed successfully!
pause