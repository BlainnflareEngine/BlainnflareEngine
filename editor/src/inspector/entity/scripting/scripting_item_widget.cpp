//
// Created by gorev on 01.12.2025.
//

#include "entity/scripting/scripting_item_widget.h"

#include "input-widgets/bool_input_field.h"
#include "input-widgets/path_input_field.h"

#include <QVBoxLayout>

namespace editor
{
scripting_item_widget::scripting_item_widget(QWidget *parent)
    : QWidget(parent)
{
    setLayout(new QVBoxLayout());
    layout()->setSpacing(5);
    layout()->setContentsMargins(5, 5, 5, 5);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    m_triggerStart = new bool_input_field("Trigger Start", true, this);
    m_scriptPath = new path_input_field("Path", QStringList{"lua"}, this);
    m_removeButton = new QPushButton("Remove", this);

    layout()->addWidget(m_triggerStart);
    layout()->addWidget(m_scriptPath);
    layout()->addWidget(m_removeButton);

    connect(m_triggerStart, &bool_input_field::toggled, this, &scripting_item_widget::OnTriggerStartChanged);
    connect(m_scriptPath, &path_input_field::PathChanged, this, &scripting_item_widget::OnScriptPathChanged);
    connect(m_removeButton, &QPushButton::clicked, this, &scripting_item_widget::OnRemoveClicked);
}


void scripting_item_widget::SetShouldTriggerStart(bool value)
{
    m_triggerStart->setChecked(value);
}


void scripting_item_widget::SetScriptPath(const QString &relativePath)
{
    m_scriptPath->SetPathSilent(relativePath);
}


bool scripting_item_widget::GetShouldTriggerStart() const
{
    return m_triggerStart->isChecked();
}


QString scripting_item_widget::GetScriptPath() const
{
    return m_scriptPath->GetPath();
}


void scripting_item_widget::OnTriggerStartChanged(bool value)
{
    emit ShouldTriggerStartChanged(value);
}


void scripting_item_widget::OnScriptPathChanged(const QString &path)
{
    emit ScriptPathChanged(path);
}


void scripting_item_widget::OnRemoveClicked()
{
    emit Removed();
}


} // namespace editor