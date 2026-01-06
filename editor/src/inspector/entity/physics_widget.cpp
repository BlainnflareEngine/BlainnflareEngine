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
    : component_widget_base(entity, "Physics", parent)
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

    LoadValues();

    connect(m_shape->comboBox(), &QComboBox::currentIndexChanged, this, &physics_widget::OnShapeChanged);
    connect(m_objectType->comboBox(), &QComboBox::currentIndexChanged, this, &physics_widget::OnObjectTypeChanged);
    connect(m_objectLayer->comboBox(), &QComboBox::currentIndexChanged, this, &physics_widget::OnObjectLayerChanged);
    connect(m_isTrigger, &bool_input_field::toggled, this, &physics_widget::OnTriggerChanged);
    connect(m_gravityFactor, &float_input_field::EditingFinished, this, &physics_widget::OnGravityChanged);
}


void physics_widget::DeleteComponent()
{
    if (m_entity.IsValid()) Blainn::PhysicsSubsystem::DestroyPhysicsComponent(m_entity);

    deleteLater();
}


void physics_widget::OnShapeChanged(int)
{
    using namespace Blainn;

    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::PhysicsComponent>())
    {
        deleteLater();
        return;
    }


    ComponentShapeType currentShapeType;
    float radius = 0.5;
    float halfHeight = 0.5;
    Vec3 extents = {0.5, 0.5, 0.5};
    {
        BodyGetter getter = PhysicsSubsystem::GetBodyGetter(m_entity);
        currentShapeType = getter.GetShapeType();

        switch (currentShapeType)
        {
        case ComponentShapeType::Sphere:
            radius = getter.GetSphereShapeRadius().value();
            break;
        case ComponentShapeType::Box:
            extents = getter.GetBoxShapeHalfExtents().value();
            break;
        case ComponentShapeType::Cylinder:
            halfHeight = getter.GetCylinderShapeHalfHeightAndRadius().value().first;
            radius = getter.GetCylinderShapeHalfHeightAndRadius().value().second;
            break;
        case ComponentShapeType::Capsule:
            halfHeight = getter.GetCapsuleShapeHalfHeightAndRadius().value().first;
            radius = getter.GetCapsuleShapeHalfHeightAndRadius().value().second;
            break;
        case ComponentShapeType::Empty:
            break;
        }
    }

    ClearSettings();
    BodyUpdater bodyUpdater = PhysicsSubsystem::GetBodyUpdater(m_entity);
    ShapeCreationSettings settings(m_shape->GetValue());
    bodyUpdater.ReplaceBodyShape(settings);


    switch (m_shape->GetValue())
    {
    case ComponentShapeType::Sphere:
        ShowSphereSettings(radius);
        bodyUpdater.SetSphereShapeSettings(m_radius->GetValue());
        break;
    case ComponentShapeType::Box:
        ShowBoxSettings(extents);
        bodyUpdater.SetBoxShapeSettings(m_extents->GetValue());
        break;
    case ComponentShapeType::Cylinder:
        ShowCylinderSettings(radius, halfHeight);
        bodyUpdater.SetCylinderShapeSettings(m_halfHeight->GetValue(), m_radius->GetValue());
        break;
    case ComponentShapeType::Capsule:
        ShowCapsuleSettings(radius, halfHeight);
        bodyUpdater.SetCapsuleShapeSettings(m_halfHeight->GetValue(), m_radius->GetValue());
        break;
    case ComponentShapeType::Empty:
        break;
    }
}

void physics_widget::LoadShape()
{
    using namespace Blainn;

    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::PhysicsComponent>())
    {
        deleteLater();
        return;
    }


    ComponentShapeType currentShapeType;
    float radius = 0.5;
    float halfHeight = 0.5;
    Vec3 extents = {0.5, 0.5, 0.5};
    {
        BodyGetter getter = PhysicsSubsystem::GetBodyGetter(m_entity);
        currentShapeType = getter.GetShapeType();

        switch (currentShapeType)
        {
        case ComponentShapeType::Sphere:
            radius = getter.GetSphereShapeRadius().value();
            break;
        case ComponentShapeType::Box:
            extents = getter.GetBoxShapeHalfExtents().value();
            break;
        case ComponentShapeType::Cylinder:
            halfHeight = getter.GetCylinderShapeHalfHeightAndRadius().value().first;
            radius = getter.GetCylinderShapeHalfHeightAndRadius().value().second;
            break;
        case ComponentShapeType::Capsule:
            halfHeight = getter.GetCapsuleShapeHalfHeightAndRadius().value().first;
            radius = getter.GetCapsuleShapeHalfHeightAndRadius().value().second;
            break;
        case ComponentShapeType::Empty:
            break;
        }
    }

    ClearSettings();
    BlockSignals(true);

    switch (currentShapeType)
    {
    case ComponentShapeType::Sphere:
        ShowSphereSettings(radius);
        break;
    case ComponentShapeType::Box:
        ShowBoxSettings(extents);
        break;
    case ComponentShapeType::Cylinder:
        ShowCylinderSettings(radius, halfHeight);
        break;
    case ComponentShapeType::Capsule:
        ShowCapsuleSettings(radius, halfHeight);
        break;
    case ComponentShapeType::Empty:
        break;
    }

    BlockSignals(false);
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
    BF_DEBUG("New layer is {}", static_cast<int>(layer));
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
    Blainn::BodyUpdater bodyUpdater = Blainn::PhysicsSubsystem::GetBodyUpdater(m_entity);
    bodyUpdater.SetMotionType(type);
    BF_DEBUG("New object type is {}", static_cast<int>(type));

    // TODO: check later
}


void physics_widget::OnGravityChanged()
{
    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::PhysicsComponent>()) return;

    BF_DEBUG("Change gravity in physics widget");
    Blainn::BodyUpdater bodyUpdater = Blainn::PhysicsSubsystem::GetBodyUpdater(m_entity);
    bodyUpdater.SetGravityFactor(m_gravityFactor->GetValue());
}


void physics_widget::OnTriggerChanged()
{
    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::PhysicsComponent>()) return;

    BF_DEBUG("Change is trigger in physics widget");
    Blainn::BodyUpdater bodyUpdater = Blainn::PhysicsSubsystem::GetBodyUpdater(m_entity);
}


void physics_widget::OnRadiusChanged()
{
    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::PhysicsComponent>()) return;

    BF_DEBUG("Change radius in physics widget");
    Blainn::BodyUpdater bodyUpdater = Blainn::PhysicsSubsystem::GetBodyUpdater(m_entity);
    bodyUpdater.SetSphereShapeSettings(m_radius->GetValue());
}


void physics_widget::OnHalfHeightChanged()
{
    using namespace Blainn;

    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::PhysicsComponent>()) return;

    BF_DEBUG("Change halfheight in physics widget");

    Blainn::BodyUpdater bodyUpdater = Blainn::PhysicsSubsystem::GetBodyUpdater(m_entity);

    if (m_shape->GetValue() == ComponentShapeType::Cylinder)
        bodyUpdater.SetCylinderShapeSettings(m_halfHeight->GetValue(), m_radius->GetValue());
    else if (m_shape->GetValue() == ComponentShapeType::Capsule)
        bodyUpdater.SetCapsuleShapeSettings(m_halfHeight->GetValue(), m_radius->GetValue());
}


void physics_widget::OnExtentsChanged()
{
    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::PhysicsComponent>()) return;

    BF_DEBUG("Change extents in physics widget");
    Blainn::BodyUpdater bodyUpdater = Blainn::PhysicsSubsystem::GetBodyUpdater(m_entity);
    bodyUpdater.SetBoxShapeSettings(m_extents->GetValue());
}


void physics_widget::ShowSphereSettings(float radius)
{
    m_radius = new float_input_field("Radius", radius, this, false);

    m_radius->SetMinValue(0.01);
    m_radius->SetDecimals(2);
    layout()->addWidget(m_radius);

    OnRadiusChanged();
    connect(m_radius, &float_input_field::EditingFinished, this, &physics_widget::OnRadiusChanged);
}


void physics_widget::ShowBoxSettings(const Blainn::Vec3 &extents)
{
    m_extents = new vector3_input_widget("Extents", extents, false, this);
    m_extents->SetMinValue(0.01);
    m_extents->SetDecimals(2);

    layout()->addWidget(m_extents);

    OnExtentsChanged();
    connect(m_extents, &vector3_input_widget::EditingFinished, this, &physics_widget::OnExtentsChanged);
}


void physics_widget::ShowCylinderSettings(float radius, float halfHeight)
{
    m_radius = new float_input_field("Radius", radius, this, false);
    m_halfHeight = new float_input_field("Half height", halfHeight, this, false);

    m_radius->SetMinValue(0.01);
    m_halfHeight->SetMinValue(0.01);

    m_halfHeight->SetDecimals(2);
    m_radius->SetDecimals(2);

    layout()->addWidget(m_radius);
    layout()->addWidget(m_halfHeight);

    OnRadiusChanged();
    OnHalfHeightChanged();
    connect(m_radius, &float_input_field::EditingFinished, this, &physics_widget::OnRadiusChanged);
    connect(m_halfHeight, &float_input_field::EditingFinished, this, &physics_widget::OnHalfHeightChanged);
}


void physics_widget::ShowCapsuleSettings(float radius, float halfHeight)
{
    m_radius = new float_input_field("Radius", radius, this, false);
    m_halfHeight = new float_input_field("Half height", halfHeight, this, false);

    m_radius->SetMinValue(0.01);
    m_halfHeight->SetMinValue(0.01);

    m_halfHeight->SetDecimals(2);
    m_radius->SetDecimals(2);

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


void physics_widget::LoadValues()
{
    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::PhysicsComponent>())
    {
        deleteLater();
        return;
    }

    auto &component = m_entity.GetComponent<Blainn::PhysicsComponent>();

    bool isTrigger;
    float gravityFactor;
    Blainn::ObjectLayer objectLayer;
    Blainn::ComponentShapeType shapeType;
    Blainn::PhysicsComponentMotionType motionType;

    {
        auto body = Blainn::PhysicsSubsystem::GetBodyGetter(m_entity);
        isTrigger = body.isTrigger();
        gravityFactor = body.GetGravityFactor();
        objectLayer = body.GetObjectLayer();
        shapeType = body.GetShapeType();
        motionType = body.GetMotionType();
    }

    m_isTrigger->setChecked(isTrigger);
    m_gravityFactor->SetValue(gravityFactor);
    m_objectLayer->SetValue(objectLayer);
    m_shape->SetValue(shapeType);
    m_objectType->SetValue(motionType);
    auto a = m_shape->GetValue();
    LoadShape();
}


void physics_widget::BlockSignals(bool value)
{
    m_isTrigger->blockSignals(value);
    m_gravityFactor->blockSignals(value);
    m_objectLayer->blockSignals(value);
    m_shape->blockSignals(value);
    m_objectType->blockSignals(value);

    if (m_radius) m_radius->blockSignals(value);
    if (m_halfHeight) m_halfHeight->blockSignals(value);
    if (m_extents) m_extents->blockSignals(value);
}
} // namespace editor