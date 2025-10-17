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
        "${CMAKE_CURRENT_SOURCE_DIR}/ui/dialog"
        "${CMAKE_CURRENT_SOURCE_DIR}/ui/inspector")

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Svg
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
        ${RESOURCES}
        src/path_bar_widget.cpp
        include/path_bar_widget.h
        ui/path_bar_widget.ui
        include/EditorSink.h
        src/console_messages_widget.cpp
        include/console_messages_widget.h
        ui/console_messages_widget.ui
        src/inspector/inspector_content_base.cpp
        include/inspector/inspector_content_base.h
        ui/inspector/inspector_content_base.ui
        src/inspector/browser_item_inspector_content.cpp
        include/inspector/browser_item_inspector_content.h
        ui/inspector/browser_item_inspector_content.ui
        src/inspector/material_inspector_content.cpp
        include/inspector/material_inspector_content.h
        ui/inspector/material_inspector_content.ui
        src/InspectorFabric.cpp
        include/InspectorFabric.h
        src/inspector/inspector_widget.cpp
        include/inspector/inspector_widget.h
        ui/inspector/inspector_widget.ui
        include/LabelsUtils.h
        src/ContentDelegate.cpp
        include/ContentDelegate.h
)

add_library(EDITOR STATIC ${EDITOR_SOURCES})

source_group(TREE "${CMAKE_SOURCE_DIR}" FILES ${EDITOR_SOURCES})


target_include_directories(${PROJECT_NAME} PUBLIC
        "${PROJECT_SOURCE_DIR}/include"
        "${PROJECT_SOURCE_DIR}/include/dialog"
        "${PROJECT_SOURCE_DIR}/include/inspector"
        "${PROJECT_SOURCE_DIR}/src"
        "${PROJECT_SOURCE_DIR}/ui"
        "${PROJECT_SOURCE_DIR}/ui/import"
        "${PROJECT_SOURCE_DIR}/ui/inspector"
        "${CMAKE_SOURCE_DIR}/common"
        "${Entt_DIR}/single_include"
)

target_link_libraries(EDITOR
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt6::Svg
        pch
        ENGINE
        qlementine
)



