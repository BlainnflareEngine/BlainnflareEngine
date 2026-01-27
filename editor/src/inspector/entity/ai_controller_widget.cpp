//
// Created by gorev on 14.01.2026.
//

#include "entity/ai_controller_widget.h"

#include "FileSystemUtils.h"
#include "Render/DebugRenderer.h"
#include "components/AIControllerComponent.h"
#include "input-widgets/bool_input_field.h"
#include "input-widgets/float_input_field.h"
#include "input-widgets/path_input_field.h"
#include "scene/EntityTemplates.h"

#include <QLayout>

namespace editor
{
ai_controller_widget::ai_controller_widget(const Blainn::Entity &entity, QWidget *parent)
    : component_widget_base(entity, "AI Controller", parent)
{
    auto comp = entity.TryGetComponent<Blainn::AIControllerComponent>();
    if (!entity.IsValid() || !comp)
    {
        deleteLater();
        return;
    }

    m_path_input = new path_input_field("Script path", {formats::behaviorFormat}, this);
    m_path_input->SetPath(ToQString(comp->scriptPath.c_str()));

    m_movementSpeed = new float_input_field("Movement speed", comp->MovementSpeed, this);
    m_movementSpeed->SetMinValue(0);
    m_movementSpeed->SetDecimals(2);
    m_movementSpeed->SetSingleStep(0.05);

    m_stoppingDistance = new float_input_field("Stopping distance", comp->StoppingDistance, this);
    m_stoppingDistance->SetMinValue(0.05);
    m_stoppingDistance->SetDecimals(2);
    m_stoppingDistance->SetSingleStep(0.05);

    m_groundOffset = new float_input_field("Ground offset", comp->GroundOffset, this);
    m_groundOffset->SetMinValue(0.0f);
    m_groundOffset->SetDecimals(2);
    m_groundOffset->SetSingleStep(0.05);

    m_faceDirection = new bool_input_field("Face movement direction", comp->FaceMovementDirection, this);

    m_rotationSpeed = new float_input_field("Rotation speed", comp->RotationSpeed, this);
    m_groundOffset->SetMinValue(0.0f);
    m_groundOffset->SetSingleStep(0.01);

    layout()->addWidget(m_path_input);
    layout()->addWidget(m_movementSpeed);
    layout()->addWidget(m_stoppingDistance);
    layout()->addWidget(m_groundOffset);
    layout()->addWidget(m_faceDirection);
    layout()->addWidget(m_rotationSpeed);

    connect(m_path_input, &path_input_field::PathChanged, this, &ai_controller_widget::OnPathChanged);
    connect(m_movementSpeed, &float_input_field::EditingFinished, this, &ai_controller_widget::OnMovementSpeedChanged);
    connect(m_stoppingDistance, &float_input_field::EditingFinished, this,
            &ai_controller_widget::OnStoppingDistanceChanged);
    connect(m_groundOffset, &float_input_field::EditingFinished, this, &ai_controller_widget::OnGroundOffsetChanged);
    connect(m_faceDirection, &bool_input_field::toggled, this, &ai_controller_widget::OnFaceMovementDirectionChanged);
    connect(m_rotationSpeed, &float_input_field::EditingFinished, this, &ai_controller_widget::OnRotationSpeedChanged);
}


void ai_controller_widget::DeleteComponent()
{
    if (m_entity.IsValid()) m_entity.RemoveComponentIfExists<Blainn::AIControllerComponent>();

    deleteLater();
}


void ai_controller_widget::OnUpdate()
{
    component_widget_base::OnUpdate();

    if (!Blainn::RenderSubsystem::GetInstance().DebugEnabled()) return;

    auto transform = m_entity.TryGetComponent<Blainn::TransformComponent>();
    if (!transform) return;

    auto controller = m_entity.TryGetComponent<Blainn::AIControllerComponent>();
    if (!controller) return;

    Blainn::Vec3 min = {transform->GetTranslation().x + 0.3f, transform->GetTranslation().y,
                        transform->GetTranslation().z + 0.3f};
    Blainn::Vec3 max = {transform->GetTranslation().x - 0.3f, transform->GetTranslation().y - controller->GroundOffset,
                        transform->GetTranslation().z - 0.3f};

    Blainn::RenderSubsystem::GetInstance().GetDebugRenderer().DrawWireBox({min}, {max}, Blainn::Color(1, 1, 0, 1));
}


void ai_controller_widget::OnPathChanged(const QString &old, const QString &path)
{
    if (!m_entity.IsValid()) return;

    auto comp = m_entity.TryGetComponent<Blainn::AIControllerComponent>();
    if (!comp) return;

    comp->scriptPath = ToString(path);
}


void ai_controller_widget::OnMovementSpeedChanged()
{
    if (!m_entity.IsValid()) return;

    auto comp = m_entity.TryGetComponent<Blainn::AIControllerComponent>();
    if (!comp) return;

    comp->MovementSpeed = m_movementSpeed->GetValue();
}


void ai_controller_widget::OnStoppingDistanceChanged()
{
    if (!m_entity.IsValid()) return;

    auto comp = m_entity.TryGetComponent<Blainn::AIControllerComponent>();
    if (!comp) return;

    comp->StoppingDistance = m_stoppingDistance->GetValue();
}


void ai_controller_widget::OnGroundOffsetChanged()
{
    if (!m_entity.IsValid()) return;

    auto comp = m_entity.TryGetComponent<Blainn::AIControllerComponent>();
    if (!comp) return;

    comp->GroundOffset = m_groundOffset->GetValue();
}


void ai_controller_widget::OnFaceMovementDirectionChanged(bool value)
{
    if (!m_entity.IsValid()) return;

    auto comp = m_entity.TryGetComponent<Blainn::AIControllerComponent>();
    if (!comp) return;

    comp->GroundOffset = value;
}


void ai_controller_widget::OnRotationSpeedChanged()
{
    if (!m_entity.IsValid()) return;

    auto comp = m_entity.TryGetComponent<Blainn::AIControllerComponent>();
    if (!comp) return;

    comp->RotationSpeed = m_rotationSpeed->GetValue();
}


void ai_controller_widget::paintEvent(QPaintEvent *event)
{
    BLAINN_PROFILE_FUNC();

    component_widget_base::paintEvent(event);
}
} // namespace editor