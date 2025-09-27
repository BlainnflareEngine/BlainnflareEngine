# Engine-specific dependencies
include_guard(GLOBAL)

set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE CACHE BOOL "")
set(FETCHCONTENT_QUIET FALSE)

include(FetchContent)

# DirectXTK12 - DirectX Toolkit for DirectX 12
FetchContent_Declare(
        DirectXTK12
        GIT_REPOSITORY https://github.com/microsoft/DirectXTK12.git
        GIT_TAG jul2025
        GIT_SHALLOW TRUE
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/DirectXTK
)
FetchContent_MakeAvailable(DirectXTK12)

# JoltPhysics - Multi-threaded rigid body physics simulation library
set(USE_STATIC_MSVC_RUNTIME_LIBRARY OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
        JoltPhysics
        GIT_REPOSITORY https://github.com/jrouwe/JoltPhysics.git
        GIT_TAG v5.3.0
        GIT_SHALLOW TRUE
        SOURCE_SUBDIR "Build"
        UPDATE_DISCONNECTED TRUE
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/JoltPhysics
)
FetchContent_MakeAvailable(JoltPhysics)

# OzzAnimation - Open source skeletal animation library
FetchContent_Declare(
        OzzAnimation
        GIT_REPOSITORY https://github.com/guillaumeblanc/ozz-animation.git
        GIT_TAG 0.16.0
        GIT_SHALLOW TRUE
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/OzzAnimation
)
FetchContent_MakeAvailable(OzzAnimation)

# Assimp - Open Asset Import Library
FetchContent_Declare(
        assimp
        GIT_REPOSITORY https://github.com/assimp/assimp.git
        GIT_TAG v6.0.2
        GIT_SHALLOW TRUE
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/assimp
)
FetchContent_MakeAvailable(assimp)

# sol2 - C++ binding library for Lua
FetchContent_Declare(
        sol2
        GIT_REPOSITORY https://github.com/ThePhD/sol2.git
        GIT_TAG v3.3.0
        GIT_SHALLOW TRUE
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/sol2
)
FetchContent_MakeAvailable(sol2)

# uuid_v4 - UUID v4 generator for C++
FetchContent_Declare(
        uuid_v4
        GIT_REPOSITORY https://github.com/crashoz/uuid_v4.git
        GIT_TAG v1.0.0
        GIT_SHALLOW TRUE
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/uuid_v4
)
# FetchContent_MakeAvailable(uuid_v4)
FetchContent_GetProperties(uuid_v4)
if (NOT ${vgjs_POPULATED})
    FetchContent_Populate(uuid_v4)
endif ()


# Set cache variables for source directories
set(FETCHCONTENT_SOURCE_DIR_DIRECTXTK12
        "${CMAKE_SOURCE_DIR}/libs/DirectXTK"
        CACHE PATH "CACHE PATH DirectXTK source dir"
)

set(FETCHCONTENT_SOURCE_DIR_JOLTPHYSICS
        "${CMAKE_SOURCE_DIR}/libs/JoltPhysics"
        CACHE PATH "CACHE PATH JoltPhysics source dir"
)

set(FETCHCONTENT_SOURCE_DIR_OZZANIMATION
        "${CMAKE_SOURCE_DIR}/libs/OzzAnimation"
        CACHE PATH "CACHE PATH OzzAnimation source dir"
)

set(FETCHCONTENT_SOURCE_DIR_ASSIMP
        "${CMAKE_SOURCE_DIR}/libs/assimp"
        CACHE PATH "CACHE PATH assimp source dir"
)

set(FETCHCONTENT_SOURCE_DIR_SOL2
        "${CMAKE_SOURCE_DIR}/libs/sol2"
        CACHE PATH "CACHE PATH sol2 source dir"
)

set(FETCHCONTENT_SOURCE_DIR_UUID_V4
        "${CMAKE_SOURCE_DIR}/libs/uuid_v4"
        CACHE PATH "CACHE PATH uuid_v4 source dir"
)


