@echo off
setlocal enabledelayedexpansion

set REPO_URL=https://github.com/BlainnflareEngine/BlainnflareEngine.git
set SOURCE_DIR=EngineSource
set BUILD_TYPE=Release
echo Downloading engine source code...

if exist "%SOURCE_DIR%\.git" (
    echo Repository exists, pulling latest changes...
    pushd "%SOURCE_DIR%"
    git pull origin main
    popd
) else (
    echo Cloning repository...
    git clone "%REPO_URL%" "%SOURCE_DIR%"
)

pushd "%SOURCE_DIR%"
echo Sourcedir - %SOURCE_DIR%

if exist "libs\clone_dependencies.bat" (
    echo Cloning dependencies...
    call libs\clone_dependencies.bat
)

if exist "build" (
    echo Removing old build directory...
    rmdir /s /q "build"
)

mkdir "build"
pushd "build"

echo Configuring with CMake...
cmake .. -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -DBLAINN_EXCLUDE_EDITOR=ON -DCMAKE_POLICY_VERSION_MINIMUM=3.5

if errorlevel 1 (
    echo CMake configure failed!
    popd & popd
    exit /b 1
)

echo Building %BUILD_TYPE% configuration...
cmake --build . --config %BUILD_TYPE% --parallel

if errorlevel 1 (
    echo Build failed!
    popd & popd
    exit /b 1
)

popd & popd

echo Copying resources to %BUILD_TYPE% folder...

set CURRENT_DIR=%CD%
echo Current directory: %CURRENT_DIR%

set TARGET_DIR=%CURRENT_DIR%\%SOURCE_DIR%\build\%BUILD_TYPE%
echo Target directory: %TARGET_DIR%

echo Checking for folders in %CURRENT_DIR%...

if exist "%CURRENT_DIR%\content" (
    echo Content folder found: %CURRENT_DIR%\content
    set CONTENT_SOURCE=%CURRENT_DIR%\content
) else (
    echo Content folder NOT found in %CURRENT_DIR%
    set CONTENT_SOURCE=
)

if exist "%CURRENT_DIR%\Config" (
    echo Config folder found: %CURRENT_DIR%\Config
    set CONFIG_SOURCE=%CURRENT_DIR%\Config
) else (
    echo Config folder NOT found in %CURRENT_DIR%
    set CONFIG_SOURCE=
)

if defined CONTENT_SOURCE (
    echo Copying content folder...
    xcopy /E /I /Y "%CONTENT_SOURCE%" "%TARGET_DIR%\content" >nul
    echo Content folder copied successfully to %TARGET_DIR%\content
) else (
    echo Warning: Content folder not found
)

if defined CONFIG_SOURCE (
    echo Copying Config folder...
    xcopy /E /I /Y "%CONFIG_SOURCE%" "%TARGET_DIR%\Config" >nul
    echo Config folder copied successfully to %TARGET_DIR%\Config
) else (
    echo Warning: Config folder not found
)

echo All done!