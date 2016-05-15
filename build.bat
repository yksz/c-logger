@echo off

set BUILD_DIR="build"

rem debug mode?
set debug=false
set dirname=%BUILD_DIR%
if "%1" == "debug" (
    set debug=true
    set dirname="%dirname%_debug"
)

rem create a directory for build
cd /d %~dp0
if not exist %dirname% (
    mkdir %dirname%
)
cd %dirname%

rem build
if %debug% == true (
    cmake -G "Visual Studio 12" ^
        -Dbuild_tests=ON ^
        -Dbuild_examples=ON ^
        ..
    cmake --build . --config Debug
) else (
    cmake -G "Visual Studio 12" ..
    cmake --build . --config Release
)

rem cleanup
cd /d %~dp0

pause
