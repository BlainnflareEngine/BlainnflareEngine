//
// Created by gorev on 29.11.2025.
//

#pragma once
#include "entity/component_widget_base.h"


class QVBoxLayout;
namespace editor
{
class scripting_item_widget;
}
class QListWidget;

namespace editor
{
class path_input_field;
class bool_input_field;
} // namespace editor
namespace editor
{

class scripting_widget : public component_widget_base
{
public:
    explicit scripting_widget(const Blainn::Entity &entity, QWidget *parent = nullptr);
    void DeleteComponent() override;

protected slots:
    void OnAddScript();
    void OnScriptItemPathChanged(const QString& oldPath, const QString &newPath);
    void OnScriptItemShouldTriggerStartChanged(const QString &path, const bool value);
    void OnScriptItemRemoved(scripting_item_widget *widget);

private:
    QVBoxLayout *m_scriptsLayout = nullptr;
    QPushButton *m_addButton = nullptr;

    void RebuildScripts();
    void SyncToEntity();
    void ConnectScriptItemSignals(scripting_item_widget *widget);

    void paintEvent(QPaintEvent *event) override;
};

} // namespace editor