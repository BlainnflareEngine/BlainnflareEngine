cmake_minimum_required(VERSION 3.1...4.1.1)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
cmake_policy(SET CMP0074 OLD)
cmake_policy(SET CMP0169 OLD)

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
        "$<$<NOT:$<OR:$<CONFIG:Dist>,$<CONFIG:Shipping>>>:TRACY_ENABLE BLAINN_HAS_CONSOLE>"
)

set(PROJECT_NAME
        BlainnflareEngine)

project(${PROJECT_NAME}
        VERSION 0.0.1
        LANGUAGES CXX
)

# cpp options
set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} main.cpp)


set(_prev "${CMAKE_FOLDER}")
set(CMAKE_FOLDER "libs")
add_subdirectory(${PROJECT_SOURCE_DIR}/libs)
set(CMAKE_FOLDER "${_prev}")

# PCH
add_library(pch INTERFACE)
target_link_libraries(pch INTERFACE
        EASTL
        spdlog
        TracyClient
        ENGINE)
target_include_directories(pch INTERFACE "${CMAKE_SOURCE_DIR}/libs/uuid_v4")
target_precompile_headers(pch INTERFACE common/pch.h)


target_compile_definitions(${PROJECT_NAME} PRIVATE BLAINN_INCLUDE_EDITOR)


#target_compile_definitions(${PROJECT_NAME} PUBLIC BLAINN_HAS_CONSOLE)

set(LIBS_DIR ${PROJECT_SOURCE_DIR}/libs)
set(ENGINE_DIR ${PROJECT_SOURCE_DIR}/engine)
set(EDITOR_DIR ${PROJECT_SOURCE_DIR}/editor)
set(JobSystem_DIR ${PROJECT_SOURCE_DIR}/libs/ViennaGameJobSystem)
add_subdirectory(${ENGINE_DIR})
add_subdirectory(${EDITOR_DIR})

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
        ${CMAKE_SOURCE_DIR}/common
        ${uuid_v4_DIR}
)



# Qt files deploy for Windows
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