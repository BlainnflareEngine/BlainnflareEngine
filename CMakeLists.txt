cmake_minimum_required(VERSION 3.1...4.1.1)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
cmake_policy(SET CMP0074 OLD)
cmake_policy(SET CMP0169 OLD)
add_compile_options(/EHsc /arch:AVX2)

option(BLAINN_EXCLUDE_EDITOR "Build the engine without the editor" OFF)
option(BLAINN_DISABLE_D3D_DEBUG_LAYER "Disable debug layer for better performance" OFF)

set(RECASTNAVIGATION_DEMO OFF CACHE BOOL "Disable RecastDemo (we don't need SDL2)" FORCE)
set(RECASTNAVIGATION_EXAMPLES OFF CACHE BOOL "Disable examples" FORCE)
set(RECASTNAVIGATION_TESTS OFF CACHE BOOL "Disable tests" FORCE)
set(RECASTNAVIGATION_STATIC ON CACHE BOOL "Build as static lib" FORCE)

# Allow VS to have the filters
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

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
        "$<$<NOT:$<OR:$<CONFIG:Dist>,$<CONFIG:Shipping>>>:BLAINN_HAS_CONSOLE>"
)

if(TRACY_ENABLE)
    add_compile_definitions("TRACY_ENABLE;BLAINN_ENABLE_PROFILING=1")
endif()

if(BLAINN_DISABLE_D3D_DEBUG_LAYER)
    add_compile_definitions("BLAINN_DISABLE_D3D_DEBUG_LAYER")
endif()

set(PROJECT_NAME
        BlainnflareEngine)

project(${PROJECT_NAME}
        VERSION 0.0.1
        LANGUAGES CXX C
)


# cpp options
set(CMAKE_CXX_STANDARD 20)


add_executable(${PROJECT_NAME} main.cpp
        common/VertexTypes.h
        common/MeshData.h
        common/ImportAssetData.h
        common/helpers.h
        engine/include/subsystems/Input/KeyboardEvents.h
        "common/AABBHelpers.h"
        icon.rc)

set (SPDLOG_WCHAR_TO_UTF8_SUPPORT ON)
add_compile_definitions("SPDLOG_WCHAR_TO_UTF8_SUPPORT")

set(JPH_DEBUG_RENDERER ON)
add_compile_definitions("JPH_DEBUG_RENDERER")

add_subdirectory(libs/EASTL)
add_subdirectory(libs/JoltPhysics/Build)
add_subdirectory(libs/spdlog)
add_subdirectory(libs/tracy)
add_subdirectory(libs/yaml-cpp)
add_subdirectory(libs/sol2)
add_subdirectory(libs/D3D12MemoryAllocator)
add_subdirectory(libs/recastnavigation)

if(NOT BLAINN_EXCLUDE_EDITOR)
    add_subdirectory(libs/qlementine)
endif()

set (BUILD_TESTING OFF)
add_subdirectory(libs/DIRECTXTK12)
add_subdirectory(libs/DIRECTXTEX)

set (ozz_build_tools OFF)
set (ozz_build_howtos OFF)
set (ozz_build_samples OFF)
set (ozz_build_tests OFF)
add_subdirectory(libs/ozz-animation)

set (ASSIMP_BUILD_TESTS OFF)
set (ASSIMP_INSTALL OFF)
add_subdirectory(libs/assimp)

# lua
add_subdirectory(libs/lua)

set(LIBS_DIR ${PROJECT_SOURCE_DIR}/libs)
set(ENGINE_DIR ${PROJECT_SOURCE_DIR}/engine)
set(EDITOR_DIR ${PROJECT_SOURCE_DIR}/editor)
set(COMMON_DIR ${CMAKE_SOURCE_DIR}/common)
set(JobSystem_DIR ${PROJECT_SOURCE_DIR}/libs/ViennaGameJobSystem)
set(Entt_DIR ${PROJECT_SOURCE_DIR}/libs/entt)
set(LUA_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/libs/lua/lua-5.4.4/src)
set(SOL2_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/libs/sol2/include)
set(IMGUI_DIR ${PROJECT_SOURCE_DIR}/libs/imgui)
set(IMGUIZMO_DIR ${PROJECT_SOURCE_DIR}/libs/ImGuizmo)

add_subdirectory(${ENGINE_DIR})

if(NOT BLAINN_EXCLUDE_EDITOR)
    add_subdirectory(${EDITOR_DIR})
endif()

# PCH
add_library(pch INTERFACE)
target_link_libraries(pch INTERFACE
        EASTL
        spdlog
        TracyClient
        yaml-cpp
        DirectXTK12
        DirectXTex
        D3D12MemoryAllocator
        Jolt
)

target_include_directories(pch INTERFACE
        "${CMAKE_SOURCE_DIR}/libs/uuid_v4"
        "${ENGINE_DIR}/include/subsystems"
        "${ENGINE_DIR}/include/tools"
        "${ENGINE_DIR}/src/subsystems"
        "${ENTT_DIR}/single_include")

target_precompile_headers(pch INTERFACE common/pch.h)


if(BLAINN_EXCLUDE_EDITOR)
    target_link_libraries(${PROJECT_NAME} PUBLIC
            pch
            ENGINE)
else()
    target_link_libraries(${PROJECT_NAME} PUBLIC
            pch
            ENGINE
            EDITOR)
endif()

set(BLAINN_COMMON_INCLUDES
        "${ENGINE_DIR}"
        "${CMAKE_SOURCE_DIR}/libs"
        "${LIBS_DIR}"
        "${JobSystem_DIR}/include"
        "${TracyClient_DIR}/public"
        "${spdlog_DIR}/include"
        "${CMAKE_SOURCE_DIR}/common"
        "${uuid_v4_DIR}"
        "${Entt_DIR}/single_include"
        $<TARGET_PROPERTY:assimp::assimp,INTERFACE_INCLUDE_DIRECTORIES>
        "${LIBS_DIR}/eventpp/include"
        "${IMGUI_DIR}"
        "${IMGUIZMO_DIR}"
)

if(NOT BLAINN_EXCLUDE_EDITOR)
    list(APPEND BLAINN_COMMON_INCLUDES "${EDITOR_DIR}")
endif()

target_include_directories(${PROJECT_NAME} PUBLIC
        ${BLAINN_COMMON_INCLUDES}
)


if(NOT BLAINN_EXCLUDE_EDITOR)
    target_compile_definitions(${PROJECT_NAME} PUBLIC BLAINN_INCLUDE_EDITOR)

    # Qt files deploy for Windows
    if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
        set(DEBUG_SUFFIX "$<$<NOT:$<OR:$<CONFIG:Dist>,$<CONFIG:Shipping>,$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>>:d>")
        set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
            if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
                set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
            endif ()
        endif ()
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${QT_INSTALL_PATH}/plugins/platforms"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")

        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${QT_INSTALL_PATH}/plugins/imageformats"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/imageformats/")

        foreach (QT_LIB Core Gui Widgets Svg)
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                    "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
        endforeach (QT_LIB)
    endif ()
endif()

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

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/content"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/content"
)