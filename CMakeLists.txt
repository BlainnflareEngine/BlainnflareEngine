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
add_compile_definitions(
        $<$<NOT:$<OR:$<CONFIG:Dist>,$<CONFIG:Shipping>>>:TRACY_ENABLE>
        $<$<NOT:$<OR:$<CONFIG:Dist>,$<CONFIG:Shipping>>>:BLAINN_HAS_CONSOLE>
)

set(PROJECT_NAME
        BlainnflareEngine)

project(${PROJECT_NAME}
        VERSION 0.0.1
        LANGUAGES CXX
)

# Include fetch files
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/FetchShared.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/FetchEngine.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/FetchEditor.cmake)

# Include project organization
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/ProjectOrganization.cmake)


# cpp options
set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} main.cpp)

# include directories
set(LIBS_DIR ${PROJECT_SOURCE_DIR}/libs)
set(ENGINE_DIR ${PROJECT_SOURCE_DIR}/engine)
set(EDITOR_DIR ${PROJECT_SOURCE_DIR}/editor)
set(JobSystem_DIR ${PROJECT_SOURCE_DIR}/libs/ViennaGameJobSystem)
set(uuid_v4_DIR ${PROJECT_SOURCE_DIR}/libs/uuid_v4)
set(Blain_common_dir ${PROJECT_SOURCE_DIR}/common)

# PCH
add_library(pch INTERFACE)
target_include_directories(pch INTERFACE
        ${uuid_v4_DIR}
)
target_link_libraries(pch INTERFACE
        EASTL
        spdlog
        TracyClient
        ENGINE)
target_precompile_headers(pch INTERFACE common/pch.h)


target_compile_definitions(${PROJECT_NAME} PRIVATE BLAINN_INCLUDE_EDITOR)


#target_compile_definitions(${PROJECT_NAME} PUBLIC BLAINN_HAS_CONSOLE)



add_subdirectory(${ENGINE_DIR})
add_subdirectory(${EDITOR_DIR})

# Dependencies are now handled by separate fetch files

target_link_libraries(${PROJECT_NAME} PUBLIC
        pch
        ENGINE
        EDITOR)

# Setup Qt deployment for the editor
setup_qt_deployment(${PROJECT_NAME})

# Organize main project structure
organize_main_project()

target_include_directories(${PROJECT_NAME} PUBLIC
        ${ENGINE_DIR}
        ${EDITOR_DIR}
        ${LIBS_DIR}
        ${JobSystem_DIR}/include
        ${TracyClient_DIR}/public
        ${spdlog_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/common
)

# Cache variables are now set in the respective fetch files


# Qt deployment is now handled in FetchEditor.cmake

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