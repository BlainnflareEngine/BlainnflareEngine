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
https://github.com/cameron314/concurrentqueue.git

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

endlocal