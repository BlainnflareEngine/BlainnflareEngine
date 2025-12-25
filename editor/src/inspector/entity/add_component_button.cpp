//
// Created by gorev on 13.11.2025.
//

#include "entity/add_component_button.h"

#include "PhysicsSubsystem.h"
#include "../../../include/inspector/entity/scripting/scripting_widget.h"
#include "ScriptingSubsystem.h"
#include "components/MeshComponent.h"
#include "components/PhysicsComponent.h"
#include "entity/mesh_widget.h"
#include "entity/physics_widget.h"
#include "entity/transform_widget.h"
#include "scene/EntityTemplates.h"

#include <QLayout>

#include "components/CameraComponent.h"
#include "entity/camera_widget.h"


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
    m_cameraAction = m_menu->addAction("Camera");

    setText("Add component");

    connect(this, &QPushButton::clicked, this, &add_component_button::OnClicked);

    connect(m_transformAction, &QAction::triggered, this, &add_component_button::OnTransformAction);
    connect(m_meshAction, &QAction::triggered, this, &add_component_button::OnMeshAction);
    connect(m_physicsAction, &QAction::triggered, this, &add_component_button::OnPhysicsAction);
    connect(m_scriptingAction, &QAction::triggered, this, &add_component_button::OnScriptingAction);
    connect(m_cameraAction, &QAction::triggered, this, &add_component_button::OnCameraAction);
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

void add_component_button::OnCameraAction()
{
    if (!m_entity.IsValid()) return;

    if (m_entity.HasComponent<Blainn::CameraComponent>()) return;
    auto& comp = m_entity.AddComponent<Blainn::CameraComponent>();
    comp.camera.Reset(75.f, 16/9.f, 0.01, 10000);

    auto camera = new camera_widget(m_entity, this);
    m_layout->insertWidget(m_layout->count() - 1, camera);
}
} // namespace editor