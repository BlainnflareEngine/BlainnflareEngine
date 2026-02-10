//
// Created by gorev on 29.11.2025.
//

#include "entity/scripting/scripting_widget.h"

#include "FileSystemUtils.h"
#include "ScriptingSubsystem.h"
#include "entity/scripting/scripting_item_widget.h"

#include <QLayout>
#include <QListWidget>
#include <QPushButton>

namespace editor
{
scripting_widget::scripting_widget(const Blainn::Entity &entity, QWidget *parent)
    : component_widget(entity, "Scripting component", parent)
{
    auto mainLayout = qobject_cast<QVBoxLayout *>(layout());
    m_scriptsLayout = new QVBoxLayout();
    m_scriptsLayout->setContentsMargins(0, 0, 0, 0);
    m_scriptsLayout->setSpacing(15);
    mainLayout->addLayout(m_scriptsLayout);

    m_addButton = new QPushButton("Add Script", this);
    connect(m_addButton, &QPushButton::clicked, this, &scripting_widget::OnAddScript);
    layout()->addWidget(m_addButton);

    RebuildScripts();
}


void scripting_widget::DeleteComponent()
{
    if (m_entity.IsValid()) Blainn::ScriptingSubsystem::DestroyScriptingComponent(m_entity);
    deleteLater();
}


void scripting_widget::OnAddScript()
{
    auto *widget = new scripting_item_widget(this);
    widget->setVisible(false);
    ConnectScriptItemSignals(widget);
    m_scriptsLayout->addWidget(widget);
    widget->setVisible(true);
}


/*void scripting_widget::OnScriptItemChanged()
{
    SyncToEntity();
}*/


void scripting_widget::OnScriptItemPathChanged(const QString &oldPath, const QString &newPath)
{
    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::ScriptingComponent>())
    {
        deleteLater();
        return;
    }

    auto &comp = m_entity.GetComponent<Blainn::ScriptingComponent>();

    auto scriptInfo = comp.scriptPaths[ToEASTLString(oldPath)];
    comp.scriptPaths.erase(ToEASTLString(oldPath));

    if (newPath.isEmpty()) return;

    comp.scriptPaths[ToEASTLString(newPath)] = scriptInfo;
}


void scripting_widget::OnScriptItemShouldTriggerStartChanged(const QString &path, const bool value)
{
    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::ScriptingComponent>())
    {
        deleteLater();
        return;
    }

    m_entity.GetComponent<Blainn::ScriptingComponent>().scriptPaths[ToEASTLString(path)].shouldTriggerStart = value;
}


void scripting_widget::OnScriptItemRemoved(scripting_item_widget *widget)
{
    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::ScriptingComponent>())
    {
        deleteLater();
        return;
    }

    m_entity.GetComponent<Blainn::ScriptingComponent>().scriptPaths.erase(ToEASTLString(widget->GetScriptPath()));
    widget->setVisible(false);
    widget->deleteLater();
}


void scripting_widget::RebuildScripts()
{
    while (QLayoutItem *item = m_scriptsLayout->takeAt(0))
    {
        if (QWidget *widget = item->widget())
        {
            widget->deleteLater();
        }
        delete item;
    }


    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::ScriptingComponent>())
    {
        deleteLater();
        return;
    }

    const auto &scripts = m_entity.GetComponent<Blainn::ScriptingComponent>().scriptPaths;

    for (const auto &[path, info] : scripts)
    {
        auto *widget = new scripting_item_widget(this);
        widget->SetShouldTriggerStart(info.shouldTriggerStart);
        widget->SetScriptPath(ToQString(path.c_str()));
        ConnectScriptItemSignals(widget);
        m_scriptsLayout->addWidget(widget);
    }
}


void scripting_widget::SyncToEntity()
{
    if (!m_entity.IsValid()) return;

    auto &comp = m_entity.GetComponent<Blainn::ScriptingComponent>();

    for (int i = 0; i < m_scriptsLayout->count(); ++i)
    {
        auto *item = m_scriptsLayout->itemAt(i);
        if (!item || !item->widget()) continue;

        auto *widget = qobject_cast<scripting_item_widget *>(item->widget());
        if (!widget) continue;

        eastl::string path = ToEASTLString(widget->GetScriptPath());

        Blainn::ScriptInfo info;
        info.shouldTriggerStart = widget->GetShouldTriggerStart();
        comp.scriptPaths[path] = info;
    }
}


void scripting_widget::ConnectScriptItemSignals(scripting_item_widget *widget)
{
    connect(widget, &scripting_item_widget::ShouldTriggerStartChanged, this,
            &scripting_widget::OnScriptItemShouldTriggerStartChanged);
    connect(widget, &scripting_item_widget::ScriptPathChanged, this, &scripting_widget::OnScriptItemPathChanged);
    connect(widget, &scripting_item_widget::Removed, this, [this, widget]() { OnScriptItemRemoved(widget); });
}


void scripting_widget::paintEvent(QPaintEvent *event)
{
    BLAINN_PROFILE_FUNC();

    component_widget::paintEvent(event);
}
} // namespace editor