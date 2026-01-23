//
// Created by gorev on 13.11.2025.
//

#include "entity/add_component_button.h"

#include "AISubsystem.h"
#include "PhysicsSubsystem.h"
#include "entity/scripting/scripting_widget.h"
#include "ScriptingSubsystem.h"
#include "../../../include/inspector/entity/perception_widget.h"
#include "components/MeshComponent.h"
#include "components/PhysicsComponent.h"
#include "components/SkyboxComponent.h"
#include "components/NavMeshVolumeComponent.h"
#include "entity/mesh_widget.h"
#include "entity/physics_widget.h"
#include "entity/skybox_widget.h"
#include "entity/transform_widget.h"
#include "entity/navmesh_volume_widget.h"
#include "scene/EntityTemplates.h"

#include <QLayout>

#include "components/CameraComponent.h"
#include "components/LightComponent.h"
#include "components/StimulusComponent.h"
#include "entity/ai_controller_widget.h"
#include "entity/camera_widget.h"
#include "entity/directional_light_widget.h"
#include "entity/stimulus_widget.h"


namespace editor
{
add_component_button::add_component_button(const Blainn::Entity &entity, QBoxLayout *layout, QWidget *parent)
    : QPushButton(parent)
    , m_entity(entity)
    , m_layout(layout)
{
    m_menu = new QMenu();

    m_transformAction = m_menu->addAction("Transform");
    m_meshAction = m_menu->addAction("Mesh");
    m_physicsAction = m_menu->addAction("Physics");
    m_scriptingAction = m_menu->addAction("Scripting");


    // AI
    m_aiMenu = m_menu->addMenu("Artificial Intelligence");
    m_aiControllerAction = m_aiMenu->addAction("AI Controller");
    m_perceptionAction = m_aiMenu->addAction("Perception");
    m_stimulusAction = m_aiMenu->addAction("Stimulus");
    m_navmeshVolumeAction = m_aiMenu->addAction("Navmesh Volume");

    // Render
    m_renderMenu = m_menu->addMenu("Render");
    m_cameraAction = m_renderMenu->addAction("Camera");
    m_skyboxAction = m_renderMenu->addAction("Skybox");

    m_renderMenu->addSeparator();
    m_directLightAction = m_renderMenu->addAction("Direct light");

    setText("Add component");

    connect(this, &QPushButton::clicked, this, &add_component_button::OnClicked);

    connect(m_transformAction, &QAction::triggered, this, &add_component_button::OnTransformAction);
    connect(m_meshAction, &QAction::triggered, this, &add_component_button::OnMeshAction);
    connect(m_physicsAction, &QAction::triggered, this, &add_component_button::OnPhysicsAction);
    connect(m_scriptingAction, &QAction::triggered, this, &add_component_button::OnScriptingAction);

    // AI
    connect(m_navmeshVolumeAction, &QAction::triggered, this, &add_component_button::OnNavmeshVolumeAction);
    connect(m_aiControllerAction, &QAction::triggered, this, &add_component_button::OnAIControllerAction);
    connect(m_stimulusAction, &QAction::triggered, this, &add_component_button::OnStimulusAction);
    connect(m_perceptionAction, &QAction::triggered, this, &add_component_button::OnPerceptionAction);


    // Render
    connect(m_cameraAction, &QAction::triggered, this, &add_component_button::OnCameraAction);
    connect(m_skyboxAction, &QAction::triggered, this, &add_component_button::OnSkyboxAction);
    connect(m_directLightAction, &QAction::triggered, this, &add_component_button::OnDirectLightAction);
}


void add_component_button::OnClicked()
{
    m_menu->setFixedWidth(width() + 20);
    m_menu->popup(mapToGlobal(rect().bottomLeft()));
}


void add_component_button::OnTransformAction()
{
    if (!m_entity.IsValid()) return;
    if (m_entity.HasComponent<Blainn::TransformComponent>()) return;

    m_entity.AddComponent<Blainn::TransformComponent>();
    auto transform = new transform_widget(m_entity, this);
    m_layout->insertWidget(m_layout->count() - 1, transform);
}


void add_component_button::OnMeshAction()
{
    if (!m_entity.IsValid()) return;
    if (m_entity.HasComponent<Blainn::MeshComponent>()) return;

    m_entity.AddComponent<Blainn::MeshComponent>(eastl::move(Blainn::AssetManager::GetDefaultMesh()));
    auto mesh = new mesh_widget(m_entity, this);
    m_layout->insertWidget(m_layout->count() - 1, mesh);
}


void add_component_button::OnPhysicsAction()
{
    if (!m_entity.IsValid()) return;
    if (m_entity.HasComponent<Blainn::PhysicsComponent>()) return;

    Blainn::PhysicsComponentSettings settings(m_entity, Blainn::ComponentShapeType::Box);
    Blainn::PhysicsSubsystem::CreateAttachPhysicsComponent(settings);
    auto physics = new physics_widget(m_entity, this);
    m_layout->insertWidget(m_layout->count() - 1, physics);
}


void add_component_button::OnScriptingAction()
{
    if (!m_entity.IsValid()) return;

    // actually CreateAttachScriptingComponent check this, but if I don't check this here, another scripting
    // widget will be created even if it already contains in UI
    if (m_entity.HasComponent<Blainn::ScriptingComponent>()) return;

    Blainn::ScriptingSubsystem::CreateAttachScriptingComponent(m_entity);
    auto scripting = new scripting_widget(m_entity, this);
    m_layout->insertWidget(m_layout->count() - 1, scripting);
}


void add_component_button::OnAIControllerAction()
{
    if (!m_entity.IsValid()) return;

    if (m_entity.HasComponent<Blainn::AIControllerComponent>()) return;

    Blainn::AISubsystem::GetInstance().CreateAttachAIControllerComponent(m_entity, "");
    auto aiController = new ai_controller_widget(m_entity, this);
    m_layout->insertWidget(m_layout->count() - 1, aiController);
}


void add_component_button::OnPerceptionAction()
{
    if (!m_entity.IsValid()) return;

    if (m_entity.HasComponent<Blainn::AIControllerComponent>()) return;

    m_entity.AddComponent<Blainn::PerceptionComponent>();
    auto aiController = new perception_widget(m_entity, this);
    m_layout->insertWidget(m_layout->count() - 1, aiController);
}


void add_component_button::OnStimulusAction()
{
    if (!m_entity.IsValid()) return;

    if (m_entity.HasComponent<Blainn::StimulusComponent>()) return;

    m_entity.AddComponent<Blainn::StimulusComponent>();
    auto stimulus = new stimulus_widget(m_entity, this);
    m_layout->insertWidget(m_layout->count() - 1, stimulus);
}


void add_component_button::OnCameraAction()
{
    if (!m_entity.IsValid()) return;

    if (m_entity.HasComponent<Blainn::CameraComponent>()) return;
    auto &comp = m_entity.AddComponent<Blainn::CameraComponent>();
    comp.camera.Reset(75.f, 16 / 9.f, 0.01, 10000);

    auto camera = new camera_widget(m_entity, this);
    m_layout->insertWidget(m_layout->count() - 1, camera);
}


void add_component_button::OnDirectLightAction()
{
    if (!m_entity.IsValid()) return;

    if (m_entity.HasComponent<Blainn::DirectionalLightComponent>()) return;

    m_entity.AddComponent<Blainn::DirectionalLightComponent>();

    auto direct = new directional_light_widget(m_entity, this);
    m_layout->insertWidget(m_layout->count() - 1, direct);
}

void add_component_button::OnSkyboxAction()
{
    if (!m_entity.IsValid()) return;

    if (m_entity.HasComponent<Blainn::SkyboxComponent>()) return;
    auto &comp = m_entity.AddComponent<Blainn::SkyboxComponent>();
    auto widget = new skybox_widget(m_entity, this);
    m_layout->insertWidget(m_layout->count() - 1, widget);
}


void add_component_button::OnNavmeshVolumeAction()
{
    if (!m_entity.IsValid()) return;

    if (m_entity.HasComponent<Blainn::NavmeshVolumeComponent>()) return;
    auto &comp = m_entity.AddComponent<Blainn::NavmeshVolumeComponent>();
    auto widget = new navmesh_volume_widget(m_entity, this);
    m_layout->insertWidget(m_layout->count() - 1, widget);
}
} // namespace editor