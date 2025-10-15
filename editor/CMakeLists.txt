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
        src/editor_settings.cpp
        src/folder_content_list_view.cpp
        src/folder_content_widget.cpp
        src/folders_tree_view.cpp
        src/folders_widget.cpp
        src/import_asset_dialog.cpp
        src/ContextMenu/ContentContextMenu.cpp
        src/ContextMenu/FileContextMenu.cpp
        include/Editor.h
        include/editor_main.h
        include/editor_settings.h
        include/FileSystemUtils.h
        include/folder_content_list_view.h
        include/folder_content_widget.h
        include/folders_tree_view.h
        include/folders_widget.h
        include/import_asset_dialog.h
        include/SettingsData.h
        include/ContextMenu/ContentContextMenu.h
        include/ContextMenu/FileContextMenu.h
        ui/editor_main.ui
        ui/editor_settings.ui
        ui/folder_content_list_view.ui
        ui/folder_content_widget.ui
        ui/folders_tree_view.ui
        ui/folders_widget.ui
        ui/import_asset_dialog.ui
        ../engine/include/scene/Scene.h
        ../engine/src/scene/Scene.cpp
        ../engine/include/scene/Entity.h
        ../engine/src/scene/Entity.cpp
        ../engine/include/scene/BasicComponents.h
        ../engine/include/scene/EntityTemplates.h)

add_library(EDITOR STATIC ${EDITOR_SOURCES})

target_sources(EDITOR PRIVATE
        include/editor_main.h
        include/Editor.h)

source_group(TREE "${CMAKE_SOURCE_DIR}" FILES ${EDITOR_SOURCES})


target_include_directories(${PROJECT_NAME} PUBLIC
        "${PROJECT_SOURCE_DIR}/include"
        "${PROJECT_SOURCE_DIR}/src"
        "${PROJECT_SOURCE_DIR}/ui"
        "${CMAKE_SOURCE_DIR}/common"
        "${Entt_DIR}/single_include"
)

target_link_libraries(EDITOR
        Qt::Core
        Qt::Gui
        Qt::Widgets
        pch
)



