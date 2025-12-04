@echo off
setlocal enabledelayedexpansion

REM List of repositories to clone
set REPOS=^
https://github.com/electronicarts/EASTL.git ^
https://github.com/microsoft/DirectXTK12.git ^
https://github.com/jrouwe/JoltPhysics.git ^
https://github.com/guillaumeblanc/ozz-animation.git ^
https://github.com/assimp/assimp.git ^
https://github.com/gabime/spdlog.git ^
https://github.com/wolfpld/tracy.git ^
https://github.com/ThePhD/sol2.git ^
https://github.com/crashoz/uuid_v4.git ^
https://github.com/jbeder/yaml-cpp.git ^
https://github.com/skypjack/entt.git ^
https://github.com/cameron314/concurrentqueue.git ^
https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator.git ^
https://github.com/wqking/eventpp.git

REM Change to the directory containing this script
cd /d "%~dp0"

REM Clone each repository if not already present
for %%R in (%REPOS%) do (
    set "REPO_URL=%%R"
    REM Extract repo name (last part after '/')
    for %%A in ("!REPO_URL!") do (
        set "REPO_NAME=%%~nxA"
    )
    REM Remove .git extension if present
    set "REPO_NAME=!REPO_NAME:.git=!"
    
    if not exist "!REPO_NAME!\" (
        echo Cloning !REPO_URL! into !REPO_NAME!...
        git clone "!REPO_URL!" "!REPO_NAME!" --depth=1
    ) else (
        echo !REPO_NAME! already exists, skipping.
    )
)

if not exist ViennaGameJobSystem (
    echo Cloning ViennaGameJobSystem into ViennaGameJobSystem
    git clone https://github.com/hlavacs/ViennaGameJobSystem.git -b master ViennaGameJobSystem
) else (
    echo ViennaGameJobSystem already exists, skipping.
)

if not exist qlementine (
    echo Cloning qlementine into qlementine
    git clone -b v1.3.0 --single-branch https://github.com/oclero/qlementine.git
)

if not exist qlementine (
    echo Cloning qlementine into qlementine
    git clone -b v1.3.0 --single-branch https://github.com/oclero/qlementine.git
)

set LUA_VERSION=5.4.4
set LUA_URL=https://www.lua.org/ftp/lua-%LUA_VERSION%.tar.gz
set LUA_DIR=%~dp0lua
set LUA_SRC_DIR=%LUA_DIR%\lua-%LUA_VERSION%
set LUA_ARCHIVE=%LUA_DIR%\lua-%LUA_VERSION%.tar.gz

:: Проверяем, существует ли директория с исходниками
if exist "%LUA_SRC_DIR%" (
    echo Lua %LUA_VERSION% already extracted in %LUA_DIR%.
    goto :done
)

echo.
echo === Downloading Lua %LUA_VERSION% ===

:: Создаём директорию lua, если её нет
if not exist "%LUA_DIR%" mkdir "%LUA_DIR%"

:: Скачиваем архив
curl -L --ssl-no-revoke -o "%LUA_ARCHIVE%" "%LUA_URL%"
if errorlevel 1 (
    echo Failed to download Lua archive.
    exit /b 1
)

echo.
echo === Extracting Lua %LUA_VERSION% ===

:: Распаковываем архив в директорию lua
tar -xzf "%LUA_ARCHIVE%" -C "%LUA_DIR%"
if errorlevel 1 (
    echo Failed to extract archive.
    exit /b 1
)

:: Удаляем архив после успешной распаковки
del "%LUA_ARCHIVE%"

echo.
echo === Lua %LUA_VERSION% is ready at %LUA_SRC_DIR% ===

:done
endlocal