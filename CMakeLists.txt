cmake_minimum_required(VERSION 3.1...4.1.1)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
cmake_policy(SET CMP0074 OLD)
cmake_policy(SET CMP0169 OLD)

set(CMAKE_CONFIGURATION_TYPES
    Debug
    Release
    RelWithDebInfo
    Development
    Shipping
    Dist
    CACHE STRING "" FORCE)

# These definitions apply to all the projects
set (NON_DIST_DEFS TRACY_ENABLE)
add_compile_definitions(
        $<$<NOT:$<OR:$<CONFIG:Dist>,$<CONFIG:Shipping>>>:${NON_DIST_DEFS}>
)

set(PROJECT_NAME
        BlainnflareEngine)

project(${PROJECT_NAME}
        VERSION 0.0.1
        LANGUAGES CXX
)

# common options
include(FetchContent)


# cpp options
set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} main.cpp)

target_compile_definitions(${PROJECT_NAME} PRIVATE BLAINN_INCLUDE_EDITOR)

# include directories

set(LIBS_DIR ${PROJECT_SOURCE_DIR}/libs)
set(ENGINE_DIR ${PROJECT_SOURCE_DIR}/engine)
set(EDITOR_DIR ${PROJECT_SOURCE_DIR}/editor)
set(JobSystem_DIR ${PROJECT_SOURCE_DIR}/libs/ViennaGameJobSystem)
add_subdirectory(${ENGINE_DIR})
add_subdirectory(${EDITOR_DIR})


# fetch content
find_package(EASTL QUIET)
if (NOT ${EASTL_FOUND})
    message(STATUS "EASTL not found. Fetching from git (https://github.com/electronicarts/EASTL.git)...")
    FetchContent_Declare(
            EASTL
            GIT_REPOSITORY https://github.com/electronicarts/EASTL.git
            GIT_TAG 3.21.23
            SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/EASTL
    )

    FetchContent_MakeAvailable(EASTL)
else ()
    message(STATUS "EASTL found.")
endif ()

find_package(spdlog QUIET)
if (NOT ${spdlog_FOUND})
    message(STATUS "spdlog not found. Fetching from git (https://github.com/gabime/spdlog.git)...")
    FetchContent_Declare(
            spdlog
            GIT_REPOSITORY https://github.com/gabime/spdlog.git
            GIT_TAG v1.15.3
            SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/spdlog
    )

    FetchContent_MakeAvailable(spdlog)
else ()
    message(STATUS "spdlog found.")
endif ()
#target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/libs/spdlog)

find_package(tracy QUIET)
if (NOT ${tracy_FOUND})
    message(STATUS "tracy not found. Fetching from git (https://github.com/wolfpld/tracy.git)...")
    FetchContent_Declare(
            tracy
            GIT_REPOSITORY https://github.com/wolfpld/tracy.git
            GIT_TAG v0.12.2
            SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/tracy
    )

    FetchContent_MakeAvailable(tracy)
else ()
    message(STATUS "tracy found.")
endif ()

find_package(DirectXTK12 QUIET)
if (NOT ${DirectXTK12_FOUND})
    message(STATUS "DirectXTK12 not found. Fetching from git (https://github.com/microsoft/DirectXTK12.git)...")
    FetchContent_Declare(
            DirectXTK12
            GIT_REPOSITORY https://github.com/microsoft/DirectXTK12.git
            GIT_TAG jul2025
            SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/DirectXTK
    )

    FetchContent_MakeAvailable(DirectXTK12)
else ()
    message(STATUS "DirectXTK12 found.")
endif ()

set(USE_STATIC_MSVC_RUNTIME_LIBRARY OFF CACHE BOOL "" FORCE)
find_package(JoltPhysics QUIET)
if (NOT ${JoltPhysics_FOUND})
    message(STATUS "JoltPhysics not found. Fetching from git (https://github.com/jrouwe/JoltPhysics.git)...")
    FetchContent_Declare(
            JoltPhysics
            GIT_REPOSITORY https://github.com/jrouwe/JoltPhysics.git
            GIT_TAG v5.3.0
            SOURCE_SUBDIR "Build"
            UPDATE_DISCONNECTED TRUE
            SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/JoltPhysics
    )

    FetchContent_MakeAvailable(JoltPhysics)
else ()
    message(STATUS "JoltPhysics found.")
endif ()

find_package(OzzAnimation QUIET)
if (NOT ${OzzAnimation_FOUND})
    message(STATUS "OzzAnimation not found. Fetching from git (https://github.com/guillaumeblanc/ozz-animation.git)...")
    FetchContent_Declare(
            OzzAnimation
            GIT_REPOSITORY https://github.com/guillaumeblanc/ozz-animation.git
            GIT_TAG 0.16.0
            SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/OzzAnimation
    )

    FetchContent_MakeAvailable(OzzAnimation)
else ()
    message(STATUS "OzzAnimation found.")
endif ()


find_package(assimp QUIET)
if (NOT ${assimp_FOUND})
    message(STATUS "Assimp not found. Fetching from git (https://github.com/assimp/assimp.git)...")
    FetchContent_Declare(
            assimp
            GIT_REPOSITORY https://github.com/assimp/assimp.git
            GIT_TAG v6.0.2
            SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/assimp
    )

    FetchContent_MakeAvailable(assimp)
else ()
    message(STATUS "Assimp found.")
endif ()

find_package(sol2 QUIET)
if (NOT ${sol2_FOUND})
    message(STATUS "sol2 not found. Fetching from git (https://github.com/ThePhD/sol2.git)...")
    FetchContent_Declare(
            sol2
            GIT_REPOSITORY https://github.com/ThePhD/sol2.git
            GIT_TAG v3.3.0
            SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/sol2
    )

    FetchContent_MakeAvailable(sol2)
else ()
    message(STATUS "sol2 found.")
endif ()

find_package(uuid_v4 QUIET)
if (NOT ${uuid_v4_FOUND})
    message(STATUS "uuid_v4 not found. Fetching from git (https://github.com/crashoz/uuid_v4.git)...")
    FetchContent_Declare(
            uuid_v4
            GIT_REPOSITORY https://github.com/crashoz/uuid_v4.git
            GIT_TAG v1.0.0
            SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/uuid_v4
    )

    FetchContent_MakeAvailable(uuid_v4)
else ()
    message(STATUS "uuid_v4 found.")
endif ()

file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/libs/ViennaGameJobSystem)

FetchContent_Declare(
        vgjs
        GIT_REPOSITORY https://github.com/hlavacs/ViennaGameJobSystem.git
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/ViennaGameJobSystem
)
FetchContent_GetProperties(vgjs)
if (NOT ${vgjs_POPULATED})
    message("Vienna Game Job System was not found, fetching")
    FetchContent_Populate(vgjs)
endif ()

target_link_libraries(${PROJECT_NAME} PUBLIC
        EASTL
        spdlog
        TracyClient
        ENGINE
        DirectXTK12
        Jolt
        ozz_animation
        assimp
        sol2)

target_include_directories(${PROJECT_NAME} PUBLIC
        ${ENGINE_DIR}
        ${EDITOR_DIR}
        ${LIBS_DIR}
        ${JobSystem_DIR}/include
        ${TracyClient}/public
)


# QT files deploy

if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    foreach (QT_LIB Core Gui Widgets)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)
endif ()

# =========================================
# debug info
if (${WIN32})
    message("Running on Windows")
elseif (${LINUX})
    message("Running on Linux")
endif ()

if (${MSVC})
    message("MSVC version: ${MSVC_VERSION}")
    message("MSVC toolset version: ${MSVC_TOOLSET_VERSION}")
endif ()

message("CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message("CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
message("CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")