cmake_minimum_required(VERSION 3.31)
project(EDITOR
        VERSION 0.0.1
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS
        "${CMAKE_CURRENT_SOURCE_DIR}/ui"
        "${CMAKE_CURRENT_SOURCE_DIR}/ui/dialog")

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

qt6_add_resources(RESOURCES sources.qrc)

set(EDITOR_SOURCES
        src/Editor.cpp
        src/editor_main.cpp
        src/editor_settings.cpp
        src/folder_content_list_view.cpp
        src/folder_content_widget.cpp
        src/folders_tree_view.cpp
        src/folders_widget.cpp
        src/dialog/import_asset_dialog.cpp
        src/context-menu/ContentContextMenu.cpp
        src/context-menu/FileContextMenu.cpp
        src/dialog/import_model_dialog.cpp
        include/Editor.h
        include/editor_main.h
        include/editor_settings.h
        include/FileSystemUtils.h
        include/folder_content_list_view.h
        include/folder_content_widget.h
        include/folders_tree_view.h
        include/folders_widget.h
        include/dialog/import_asset_dialog.h
        include/SettingsData.h
        include/context-menu/ContentContextMenu.h
        include/context-menu/FileContextMenu.h
        include/dialog/import_model_dialog.h
        src/FileSystemUtils.cpp
        src/dialog/create_material_dialog.cpp
        include/dialog/create_material_dialog.h
        ui/dialog/create_material_dialog.ui
        src/IconProvider.cpp
        include/IconProvider.h
        ${RESOURCES})

add_library(EDITOR STATIC ${EDITOR_SOURCES})

source_group(TREE "${CMAKE_SOURCE_DIR}" FILES ${EDITOR_SOURCES})


target_include_directories(${PROJECT_NAME} PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/include/dialog
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/ui
        ${PROJECT_SOURCE_DIR}/ui/import
        ${CMAKE_SOURCE_DIR}/common
)

target_link_libraries(EDITOR
        Qt::Core
        Qt::Gui
        Qt::Widgets
        pch
        ENGINE
)



