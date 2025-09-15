@echo off

set "VS_GENERATOR=Visual Studio 17 2022"
set "BUILD_DIR=build"
set "SOURCE_DIR=."
set "CONFIG=debug"

if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

cmake -B "%BUILD_DIR%" -G "%VS_GENERATOR%" -A x64
cmake --build "%BUILD_DIR%" --config %CONFIG%

echo Build success
pause