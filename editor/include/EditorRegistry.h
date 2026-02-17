//
// Created by gorev on 26.01.2026.
//

#pragma once
#include "EASTL/internal/function.h"
#include "components/AIControllerComponent.h"
#include "components/CameraComponent.h"
#include "components/LightComponent.h"
#include "components/MeshComponent.h"
#include "components/NavMeshVolumeComponent.h"
#include "components/PhysicsComponent.h"
#include "components/PrefabComponent.h"
#include "components/ScriptingComponent.h"
#include "components/SkyboxComponent.h"
#include "components/StimulusComponent.h"
#include "scene/Entity.h"

#include "entity/physics_widget.h"
#include "entity/camera_widget.h"
#include "entity/ai_controller_widget.h"
#include "entity/component_widget_base.h"
#include "entity/mesh_widget.h"
#include "entity/navmesh_volume_widget.h"
#include "entity/perception_widget.h"
#include "entity/prefab_widget.h"
#include "entity/skybox_widget.h"
#include "entity/stimulus_widget.h"
#include "entity/transform_widget.h"
#include "entity/light/spot_light_widget.h"
#include "entity/light/directional_light_widget.h"
#include "entity/light/point_light_widget.h"
#include "entity/scripting/scripting_widget.h"

#include <QWidget>


namespace editor
{

using WidgetFactoryFn = eastl::function<component_widget_base *(Blainn::Entity, QWidget *)>;
eastl::vector<eastl::pair<entt::id_type, WidgetFactoryFn>> &GetWidgetRegistry();

template <typename ComponentType, typename WidgetType> void RegisterComponentWidget()
{
    auto &widgetRegistry = GetWidgetRegistry();
    widgetRegistry.emplace_back(entt::type_hash<ComponentType>::value(),
                                [](Blainn::Entity entity, QWidget *parent) -> component_widget_base *
                                { return new WidgetType(entity, parent); });
}

inline void InitializeEditorWidgets()
{
    using namespace Blainn;

    RegisterComponentWidget<PrefabComponent, prefab_widget>();
    RegisterComponentWidget<TransformComponent, transform_widget>();
    RegisterComponentWidget<SpotLightComponent, spot_light_widget>();
    RegisterComponentWidget<PointLightComponent, point_light_widget>();
    RegisterComponentWidget<DirectionalLightComponent, directional_light_widget>();
    RegisterComponentWidget<PhysicsComponent, physics_widget>();
    RegisterComponentWidget<CameraComponent, camera_widget>();
    RegisterComponentWidget<StimulusComponent, stimulus_widget>();
    RegisterComponentWidget<SkyboxComponent, skybox_widget>();
    RegisterComponentWidget<ScriptingComponent, scripting_widget>();
    RegisterComponentWidget<NavmeshVolumeComponent, navmesh_volume_widget>();
    RegisterComponentWidget<MeshComponent, mesh_widget>();
    RegisterComponentWidget<AIControllerComponent, ai_controller_widget>();
    RegisterComponentWidget<PerceptionComponent, perception_widget>();
}
} // namespace editor
