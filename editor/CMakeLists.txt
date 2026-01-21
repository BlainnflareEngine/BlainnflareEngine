cmake_minimum_required(VERSION 3.31)
project(EDITOR
        VERSION 0.0.1
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS
        "${CMAKE_CURRENT_SOURCE_DIR}/ui"
        "${CMAKE_CURRENT_SOURCE_DIR}/ui/dialog"
        "${CMAKE_CURRENT_SOURCE_DIR}/ui/inspector"
        "${CMAKE_CURRENT_SOURCE_DIR}/ui/scene"
        "${PROJECT_SOURCE_DIR}/ui/content-browser"
        "${PROJECT_SOURCE_DIR}/ui/content-browser/folders"
        "${PROJECT_SOURCE_DIR}/ui/content-browser/folder-content")

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
        src/content-browser/folder-content/folder_content_list_view.cpp
        src/content-browser/folder-content/folder_content_widget.cpp
        src/content-browser/folders/folders_tree_view.cpp
        src/content-browser/folders/folders_widget.cpp
        src/dialog/import_asset_dialog.cpp
        src/context-menu/ContentContextMenu.cpp
        src/context-menu/FileContextMenu.cpp
        src/dialog/import_model_dialog.cpp
        include/Editor.h
        include/editor_main.h
        include/editor_settings.h
        include/FileSystemUtils.h
        include/content-browser/folder-content/folder_content_list_view.h
        include/content-browser/folder-content/folder_content_widget.h
        include/content-browser/folders/folders_tree_view.h
        include/content-browser/folders/folders_widget.h
        include/dialog/import_asset_dialog.h
        include/SettingsData.h
        ui/editor_main.ui
        ui/editor_settings.ui
        ui/content-browser/folder-content/folder_content_list_view.ui
        ui/content-browser/folder-content/folder_content_widget.ui
        ui/content-browser/folders/folders_tree_view.ui
        ui/content-browser/folders/folders_widget.ui
        include/context-menu/ContentContextMenu.h
        include/context-menu/FileContextMenu.h
        include/dialog/import_model_dialog.h
        src/FileSystemUtils.cpp
        src/dialog/create_material_dialog.cpp
        include/dialog/create_material_dialog.h
        src/content-browser/IconProvider.cpp
        include/content-browser/IconProvider.h
        ${RESOURCES}
        src/content-browser/path_bar_widget.cpp
        include/content-browser/path_bar_widget.h
        ui/content-browser/path_bar_widget.ui
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
        src/inspector/InspectorFabric.cpp
        include/inspector/InspectorFabric.h
        src/inspector/inspector_widget.cpp
        include/inspector/inspector_widget.h
        ui/inspector/inspector_widget.ui
        include/LabelsUtils.h
        src/content-browser/folder-content/ContentDelegate.cpp
        include/content-browser/folder-content/ContentDelegate.h
        src/dialog/import_texture_dialog.cpp
        include/dialog/import_texture_dialog.h
        ui/dialog/import_texture_dialog.ui
        src/inspector/mesh_inspector_content.cpp
        include/inspector/mesh_inspector_content.h
        ui/inspector/mesh_inspector_content.ui
        src/content-browser/folder-content/ContentFilterProxyModel.cpp
        include/content-browser/folder-content/ContentFilterProxyModel.h
        src/context-menu/SceneContextMenu.cpp
        include/context-menu/SceneContextMenu.h
        src/scene/scene_hierarchy_widget.cpp
        include/scene/scene_hierarchy_widget.h
        ui/scene/scene_hierarchy_widget.ui
        include/scene/EntityNode.h
        ui/scene/SceneItemModel.cpp
        include/scene/SceneItemModel.h
        src/scene/EntityDelegate.cpp
        include/scene/EntityDelegate.h
        src/TexturePreviewAsyncLoader.cpp
        include/TexturePreviewAsyncLoader.h
        src/inspector/entity_inspector_content.cpp
        include/inspector/entity_inspector_content.h
        include/inspector/EntityInspectorData.h
        include/inspector/entity/add_component_button.h
        src/inspector/entity/add_component_button.cpp
        ../engine/include/scene/SceneParser.h
        src/inspector/entity/transform_widget.cpp
        include/inspector/entity/transform_widget.h
        src/inspector/input-widgets/float_input_field.cpp
        include/inspector/input-widgets/float_input_field.h
        include/themed_panel.h
        src/themed_panel.cpp
        src/inspector/entity/component_widget_base.cpp
        include/inspector/entity/component_widget_base.h
        src/inspector/entity/mesh_widget.cpp
        include/inspector/entity/mesh_widget.h
        src/inspector/input-widgets/path_input_field.cpp
        include/inspector/input-widgets/path_input_field.h
        include/inspector/input-widgets/NumericInputWidget.h
        src/scene/SceneMeta.cpp
        include/scene/SceneMeta.h
        include/scene/EntityNodeComparator.h
        src/inspector/input-widgets/bool_input_field.cpp
        include/inspector/input-widgets/bool_input_field.h
        src/inspector/entity/scripting/scripting_widget.cpp
        include/inspector/entity/scripting/scripting_widget.h
        src/inspector/entity/scripting/scripting_item_widget.cpp
        include/inspector/entity/scripting/scripting_item_widget.h
        include/viewport_widget.h
        src/inspector/entity/physics_widget.cpp
        include/inspector/entity/physics_widget.h
        include/inspector/input-widgets/enum_input_widget.h
        include/inspector/input-widgets/EnumTemplates.h
        src/inspector/input-widgets/vector3_input_widget.cpp
        include/inspector/input-widgets/vector3_input_widget.h
        src/inspector/input-widgets/color_input_field.cpp
        include/inspector/input-widgets/color_input_field.h
        ../engine/include/components/CameraComponent.h
        src/inspector/entity/camera_widget.cpp
        include/inspector/entity/camera_widget.h
        include/inspector/entity/skybox_widget.h
        src/inspector/entity/skybox_widget.cpp
        src/inspector/entity/ai_controller_widget.cpp
        include/inspector/entity/ai_controller_widget.h
        include/inspector/entity/navmesh_volume_widget.h
        src/inspector/entity/navmesh_volume_widget.cpp
        include/inspector/entity/stimulus_widget.h
        src/inspector/entity/stimulus_widget.cpp
        include/inspector/input-widgets/string_input_field.h
        src/inspector/input-widgets/collapsible_group.cpp
        include/inspector/input-widgets/collapsible_group.h
        src/inspector/entity/perception_widget.cpp
        include/inspector/entity/perception_widget.h
        include/inspector/input-widgets/string_list_input.h
        src/inspector/input-widgets/vector3_bool_widget.cpp
        include/inspector/input-widgets/vector3_bool_widget.h
)

add_library(EDITOR STATIC ${EDITOR_SOURCES})

source_group(TREE "${CMAKE_SOURCE_DIR}" FILES ${EDITOR_SOURCES})


target_include_directories(${PROJECT_NAME} PUBLIC
        "${PROJECT_SOURCE_DIR}/include"
        "${PROJECT_SOURCE_DIR}/include/dialog"
        "${PROJECT_SOURCE_DIR}/include/inspector"
        "${PROJECT_SOURCE_DIR}/include/content-browser"
        "${PROJECT_SOURCE_DIR}/include/content-browser/folders"
        "${PROJECT_SOURCE_DIR}/include/content-browser/folder-content"
        "${PROJECT_SOURCE_DIR}/include/scene"
        "${PROJECT_SOURCE_DIR}/src"
        "${PROJECT_SOURCE_DIR}/ui"
        "${PROJECT_SOURCE_DIR}/ui/content-browser/folder-content"
        "${PROJECT_SOURCE_DIR}/ui/import"
        "${PROJECT_SOURCE_DIR}/ui/inspector"
        "${PROJECT_SOURCE_DIR}/ui/scene"
        "${CMAKE_SOURCE_DIR}/common"
        "${CMAKE_SOURCE_DIR}/libs/"
        "${Entt_DIR}/single_include"
        "${CMAKE_SOURCE_DIR}/libs/eventpp/include"
)

target_link_libraries(EDITOR
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Svg
        pch
        ENGINE
        qlementine
        Jolt
)



