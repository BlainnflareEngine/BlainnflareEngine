//
// Created by gorev on 14.01.2026.
//

#include "entity/ai_controller_widget.h"

#include "FileSystemUtils.h"
#include "components/AIControllerComponent.h"
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

    m_stoppingDistance = new float_input_field("Stopping distance", comp->StoppingDistance, this);
    m_stoppingDistance->SetMinValue(0.05);
    m_stoppingDistance->SetDecimals(2);

    layout()->addWidget(m_path_input);
    layout()->addWidget(m_movementSpeed);
    layout()->addWidget(m_stoppingDistance);

    connect(m_path_input, &path_input_field::PathChanged, this, &ai_controller_widget::OnPathChanged);
    connect(m_movementSpeed, &float_input_field::EditingFinished, this, &ai_controller_widget::OnMovementSpeedChanged);
    connect(m_stoppingDistance, &float_input_field::EditingFinished, this,
            &ai_controller_widget::OnStoppingDistanceChanged);
}


void ai_controller_widget::DeleteComponent()
{
    if (m_entity.IsValid()) m_entity.RemoveComponentIfExists<Blainn::AIControllerComponent>();

    deleteLater();
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
} // namespace editor