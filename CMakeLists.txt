cmake_minimum_required(VERSION 3.21...4.0.1)

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

add_executable(${PROJECT_NAME} main.cc)

# include directories

set(LIBS_DIR ${PROJECT_SOURCE_DIR}/libs)
set(ENGINE_DIR ${PROJECT_SOURCE_DIR}/engine)
set(EDITOR_DIR ${PROJECT_SOURCE_DIR}/editor)
set(JobSystem_DIR ${PROJECT_SOURCE_DIR}/libs/JobSystem)
add_subdirectory(engine)

target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_DIR} ${EDITOR_DIR} ${LIBS_DIR} ${JobSystem_DIR})

# fetch content
find_package(EASTL QUIET)
if (NOT EASTL_FOUND)
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

find_package(DirectXTK12 QUIET)
if (NOT DirectXTK12_FOUND)
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

find_package(JoltPhysics QUIET)
if (NOT JoltPhysics_FOUND)
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
if (NOT OzzAnimation_FOUND)
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
if (NOT assimp_FOUND)
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


target_link_libraries(${PROJECT_NAME} PUBLIC
        EASTL
        ENGINE
        DirectXTK12
        Jolt
        ozz_animation
        assimp)


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