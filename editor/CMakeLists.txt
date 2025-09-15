cmake_minimum_required(VERSION 3.31)
project(EDITOR
        VERSION 0.0.1
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/ui")

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

set(EDITOR_SOURCES
        src/Editor.cpp
        src/editor_main.cpp)

add_library(EDITOR STATIC ${EDITOR_SOURCES})

target_sources(EDITOR PRIVATE
        include/editor_main.h
        include/Editor.h)


target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/ui)

target_link_libraries(EDITOR
        Qt::Core
        Qt::Gui
        Qt::Widgets
)



