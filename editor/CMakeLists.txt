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
        src/editor_main.cpp
        src/folders_widget.cpp
        include/folders_widget.h
        ui/folders_widget.ui
        src/folder_content_widget.cpp
        include/folder_content_widget.h
        ui/folder_content_widget.ui
        src/ContextMenu/ContentContextMenu.cpp
        include/ContextMenu/ContentContextMenu.h
        src/ContextMenu/FileContextMenu.cpp
        include/ContextMenu/FileContextMenu.h
        include/FileSystemUtils.h
        src/folder_content_list_view.cpp
        include/folder_content_list_view.h
        ui/folder_content_list_view.ui
        src/folders_tree_view.cpp
        include/folders_tree_view.h
        ui/folders_tree_view.ui
        src/import_asset_dialog.cpp
        include/import_asset_dialog.h
        ui/import_asset_dialog.ui
        include/editor_main.h
        include/Editor.h
        src/editor_settings.cpp
        include/editor_settings.h
        ui/editor_settings.ui
        include/SettingsData.h)

add_library(EDITOR STATIC ${EDITOR_SOURCES})


target_include_directories(${PROJECT_NAME} PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/ui
)

target_link_libraries(EDITOR
        Qt::Core
        Qt::Gui
        Qt::Widgets
        pch
        yaml-cpp
)



