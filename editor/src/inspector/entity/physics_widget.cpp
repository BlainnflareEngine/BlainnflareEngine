//
// Created by gorev on 07.12.2025.
//

#include "entity/physics_widget.h"

#include "PhysicsSubsystem.h"
#include "components/PhysicsComponent.h"
#include "input-widgets/bool_input_field.h"
#include "input-widgets/float_input_field.h"
#include "input-widgets/vector3_input_widget.h"

#include <QLayout>

namespace editor
{
physics_widget::physics_widget(const Blainn::Entity &entity, QWidget *parent)
    : component_widget_base(entity, "Physics widget", parent)
{
    m_isTrigger = new bool_input_field("Is Trigger", false, this);

    m_gravityFactor = new float_input_field("Gravity", 1.0f, this);

    m_shape = new enum_input_widget<Blainn::ComponentShapeType>("Shape type", this);
    m_objectType = new enum_input_widget<Blainn::PhysicsComponentMotionType>("Object type", this);
    m_objectLayer = new enum_input_widget<Blainn::ObjectLayer>("Object layer", this);

    layout()->addWidget(m_isTrigger);
    layout()->addWidget(m_gravityFactor);
    layout()->addWidget(m_shape);
    layout()->addWidget(m_objectType);
    layout()->addWidget(m_objectLayer);

    connect(m_shape->comboBox(), &QComboBox::currentIndexChanged, this, &physics_widget::OnShapeChanged);
    connect(m_objectType->comboBox(), &QComboBox::currentIndexChanged, this, &physics_widget::OnObjectTypeChanged);
    connect(m_objectLayer->comboBox(), &QComboBox::currentIndexChanged, this, &physics_widget::OnObjectLayerChanged);
    connect(m_isTrigger, &bool_input_field::toggled, this, &physics_widget::OnTriggerChanged);
    connect(m_gravityFactor, &float_input_field::EditingFinished, this, &physics_widget::OnGravityChanged);

    OnShapeChanged(0);
}


void physics_widget::DeleteComponent()
{
    if (m_entity.IsValid()) Blainn::PhysicsSubsystem::DestroyPhysicsComponent(m_entity);

    deleteLater();
}


void physics_widget::OnShapeChanged(int)
{
    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::PhysicsComponent>())
    {
        deleteLater();
        return;
    }

    Blainn::ComponentShapeType shape = m_shape->GetValue();

    ClearSettings();

    switch (shape)
    {
    case Blainn::ComponentShapeType::Sphere:
        ShowSphereSettings();
        break;
    case Blainn::ComponentShapeType::Box:
        ShowBoxSettings();
        break;
    case Blainn::ComponentShapeType::Cylinder:
        ShowCylinderSettings();
        break;
    case Blainn::ComponentShapeType::Capsule:
        ShowCapsuleSettings();
        break;
    case Blainn::ComponentShapeType::Empty:
        break;
    }

    auto &comp = m_entity.GetComponent<Blainn::PhysicsComponent>();
    comp.shapeType = shape;
    // TODO: check later
}


void physics_widget::OnObjectLayerChanged(int)
{
    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::PhysicsComponent>())
    {
        deleteLater();
        return;
    }

    Blainn::ObjectLayer layer = m_objectLayer->GetValue();

    Blainn::BodyUpdater bodyUpdater = Blainn::PhysicsSubsystem::GetBodyUpdater(m_entity);
    bodyUpdater.SetObjectLayer(layer);

    // TODO: check later
}


void physics_widget::OnObjectTypeChanged(int)
{
    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::PhysicsComponent>())
    {
        deleteLater();
        return;
    }

    Blainn::PhysicsComponentMotionType type = m_objectType->GetValue();

    // TODO: check later
}


void physics_widget::OnGravityChanged()
{
    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::PhysicsComponent>()) return;

    BF_DEBUG("Change gravity in physics widget");
}


void physics_widget::OnTriggerChanged()
{
    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::PhysicsComponent>()) return;

    BF_DEBUG("Change is trigger in physics widget");
}


void physics_widget::OnRadiusChanged()
{
    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::PhysicsComponent>()) return;

    BF_DEBUG("Change radius in physics widget");
}


void physics_widget::OnHalfHeightChanged()
{
    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::PhysicsComponent>()) return;

    BF_DEBUG("Change halfheight in physics widget");
}


void physics_widget::OnExtentsChanged()
{
    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::PhysicsComponent>()) return;

    BF_DEBUG("Change extents in physics widget");
}


void physics_widget::ShowSphereSettings()
{
    m_radius = new float_input_field("Radius", 1, this);

    layout()->addWidget(m_radius);

    OnRadiusChanged();
    connect(m_radius, &float_input_field::EditingFinished, this, &physics_widget::OnRadiusChanged);
}


void physics_widget::ShowBoxSettings()
{
    m_extents = new vector3_input_widget("Extents", {0.5f, 0.5f, 0.5f}, this);

    layout()->addWidget(m_extents);

    OnExtentsChanged();
    connect(m_extents, &vector3_input_widget::EditingFinished, this, &physics_widget::OnExtentsChanged);
}


void physics_widget::ShowCylinderSettings()
{
    m_radius = new float_input_field("Radius", 1, this);
    m_halfHeight = new float_input_field("Half height", 0.5, this);

    layout()->addWidget(m_radius);
    layout()->addWidget(m_halfHeight);

    OnRadiusChanged();
    OnHalfHeightChanged();
    connect(m_radius, &float_input_field::EditingFinished, this, &physics_widget::OnRadiusChanged);
    connect(m_halfHeight, &float_input_field::EditingFinished, this, &physics_widget::OnHalfHeightChanged);
}


void physics_widget::ShowCapsuleSettings()
{
    m_radius = new float_input_field("Radius", 1, this);
    m_halfHeight = new float_input_field("Half height", 0.5, this);

    layout()->addWidget(m_radius);
    layout()->addWidget(m_halfHeight);

    OnRadiusChanged();
    OnHalfHeightChanged();
    connect(m_radius, &float_input_field::EditingFinished, this, &physics_widget::OnRadiusChanged);
    connect(m_halfHeight, &float_input_field::EditingFinished, this, &physics_widget::OnHalfHeightChanged);
}


void physics_widget::ClearSettings()
{
    if (m_radius)
    {
        layout()->removeWidget(m_radius);
        m_radius->deleteLater();
        m_radius = nullptr;
    }

    if (m_halfHeight)
    {
        layout()->removeWidget(m_halfHeight);
        m_halfHeight->deleteLater();
        m_halfHeight = nullptr;
    }

    if (m_extents)
    {
        layout()->removeWidget(m_extents);
        m_extents->deleteLater();
        m_extents = nullptr;
    }
}
} // namespace editor