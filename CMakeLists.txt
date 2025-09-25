cmake_minimum_required(VERSION 3.1...4.1.1)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
cmake_policy(SET CMP0074 OLD)
cmake_policy(SET CMP0169 OLD)
add_compile_options(/EHsc)

set(CMAKE_CONFIGURATION_TYPES
        Debug
        Release
        RelWithDebInfo
        Development
        Shipping
        Dist
        CACHE STRING "" FORCE)

# These definitions apply to all the projects
set(NON_DIST_DEFS TRACY_ENABLE)
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

# add content folder
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Content)


# cpp options
set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} main.cpp)

# PCH
add_library(pch INTERFACE)
target_link_libraries(pch INTERFACE
        EASTL
        spdlog
        TracyClient
        ENGINE)
target_precompile_headers(pch INTERFACE common/pch.h)


target_compile_definitions(${PROJECT_NAME} PRIVATE BLAINN_INCLUDE_EDITOR)


#target_compile_definitions(${PROJECT_NAME} PUBLIC BLAINN_HAS_CONSOLE)

# include directories

set(LIBS_DIR ${PROJECT_SOURCE_DIR}/libs)
set(ENGINE_DIR ${PROJECT_SOURCE_DIR}/engine)
set(EDITOR_DIR ${PROJECT_SOURCE_DIR}/editor)
set(JobSystem_DIR ${PROJECT_SOURCE_DIR}/libs/ViennaGameJobSystem)
add_subdirectory(${ENGINE_DIR})
add_subdirectory(${EDITOR_DIR})

# fetch content
FetchContent_Declare(
        EASTL
        GIT_REPOSITORY https://github.com/electronicarts/EASTL.git
        GIT_TAG 3.21.23
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/EASTL
)

FetchContent_MakeAvailable(EASTL)

FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.15.3
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/spdlog
)

FetchContent_MakeAvailable(spdlog)


FetchContent_Declare(
        tracy
        GIT_REPOSITORY https://github.com/wolfpld/tracy.git
        GIT_TAG v0.12.2
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/tracy
)

FetchContent_MakeAvailable(tracy)


FetchContent_Declare(
        DirectXTK12
        GIT_REPOSITORY https://github.com/microsoft/DirectXTK12.git
        GIT_TAG jul2025
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/DirectXTK
)

FetchContent_MakeAvailable(DirectXTK12)


set(USE_STATIC_MSVC_RUNTIME_LIBRARY OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
        JoltPhysics
        GIT_REPOSITORY https://github.com/jrouwe/JoltPhysics.git
        GIT_TAG v5.3.0
        SOURCE_SUBDIR "Build"
        UPDATE_DISCONNECTED TRUE
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/JoltPhysics
)

FetchContent_MakeAvailable(JoltPhysics)

FetchContent_Declare(
        OzzAnimation
        GIT_REPOSITORY https://github.com/guillaumeblanc/ozz-animation.git
        GIT_TAG 0.16.0
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/OzzAnimation
)

FetchContent_MakeAvailable(OzzAnimation)


FetchContent_Declare(
        assimp
        GIT_REPOSITORY https://github.com/assimp/assimp.git
        GIT_TAG v6.0.2
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/assimp
)

FetchContent_MakeAvailable(assimp)


FetchContent_Declare(
        sol2
        GIT_REPOSITORY https://github.com/ThePhD/sol2.git
        GIT_TAG v3.3.0
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/sol2
)

FetchContent_MakeAvailable(sol2)


FetchContent_Declare(
        uuid_v4
        GIT_REPOSITORY https://github.com/crashoz/uuid_v4.git
        GIT_TAG v1.0.0
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/uuid_v4
)

FetchContent_MakeAvailable(uuid_v4)


file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/libs/ViennaGameJobSystem)

FetchContent_Declare(
        vgjs
        GIT_REPOSITORY https://github.com/hlavacs/ViennaGameJobSystem.git
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/ViennaGameJobSystem
)
FetchContent_GetProperties(vgjs)
if (NOT ${vgjs_POPULATED})
    FetchContent_Populate(vgjs)
endif ()

target_link_libraries(${PROJECT_NAME} PUBLIC
        pch
        ENGINE
        EDITOR)

target_include_directories(${PROJECT_NAME} PUBLIC
        ${ENGINE_DIR}
        ${EDITOR_DIR}
        ${LIBS_DIR}
        ${JobSystem_DIR}/include
        ${TracyClient_DIR}/public
        ${spdlog_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/common
)

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

set(FETCHCONTENT_SOURCE_DIR_VGJS
        "${CMAKE_SOURCE_DIR}/libs/ViennaGameJobSystem"
        CACHE PATH "CACHE PATH vgjs source dir"
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