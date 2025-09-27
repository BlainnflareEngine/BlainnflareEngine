# Shared dependencies for both engine and editor
include_guard(GLOBAL)

set(FETCHCONTENT_BASE_DIR ${CMAKE_SOURCE_DIR}/libs CACHE PATH "Dependencies directory" FORCE)
set(FETCHCONTENT_FULLY_DISCONNECTED TRUE CACHE BOOL "")
set(FETCHCONTENT_QUIET FALSE)

include(FetchContent)


# EASTL - Electronic Arts Standard Template Library
FetchContent_Declare(
        EASTL
        GIT_REPOSITORY https://github.com/electronicarts/EASTL.git
        GIT_TAG 3.21.23
        GIT_SHALLOW TRUE
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/EASTL
)
FetchContent_MakeAvailable(EASTL)

# spdlog - Fast C++ logging library
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.15.3
        GIT_SHALLOW TRUE
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/spdlog
)
FetchContent_MakeAvailable(spdlog)

# Tracy - Real-time, nanosecond resolution, remote telemetry, hybrid frame and memory profiler
FetchContent_Declare(
        tracy
        GIT_REPOSITORY https://github.com/wolfpld/tracy.git
        GIT_TAG v0.12.2
        GIT_SHALLOW TRUE
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/tracy
)
FetchContent_MakeAvailable(tracy)

# ViennaGameJobSystem - Job system for game engines
file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/libs/ViennaGameJobSystem)
FetchContent_Declare(
        vgjs
        GIT_REPOSITORY https://github.com/hlavacs/ViennaGameJobSystem.git
        GIT_SHALLOW TRUE
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/ViennaGameJobSystem
)
FetchContent_GetProperties(vgjs)
if (NOT ${vgjs_POPULATED})
    FetchContent_Populate(vgjs)
endif ()

# Set cache variables for source directories
set(FETCHCONTENT_SOURCE_DIR_EASTL
        "${CMAKE_SOURCE_DIR}/libs/EASTL"
        CACHE PATH "CACHE PATH EASTL source dir"
)

set(FETCHCONTENT_SOURCE_DIR_SPDLOG
        "${CMAKE_SOURCE_DIR}/libs/spdlog"
        CACHE PATH "CACHE PATH SPDLOG source dir"
)

set(FETCHCONTENT_SOURCE_DIR_TRACY
        "${CMAKE_SOURCE_DIR}/libs/tracy"
        CACHE PATH "CACHE PATH tracy source dir"
)

set(FETCHCONTENT_SOURCE_DIR_VGJS
        "${CMAKE_SOURCE_DIR}/libs/ViennaGameJobSystem"
        CACHE PATH "CACHE PATH vgjs source dir"
)