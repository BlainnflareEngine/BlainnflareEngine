//
// Created by gorev on 01.12.2025.
//

#pragma once
#include "themed_panel.h"


#include <QPushButton>
#include <QWidget>

namespace editor
{
class path_input_field;
}
namespace editor
{
class bool_input_field;
}
namespace editor
{

class scripting_item_widget : public themed_panel
{
    Q_OBJECT

public:
    explicit scripting_item_widget(QWidget *parent = nullptr);

    void SetShouldTriggerStart(bool value);
    void SetScriptPath(const QString &relativePath);

    bool GetShouldTriggerStart() const;
    QString GetScriptPath() const;

signals:
    void ShouldTriggerStartChanged(const QString &path, const bool value);
    void ScriptPathChanged(const QString &oldPath, const QString &newPath);
    void Removed();

private slots:
    void OnTriggerStartChanged(bool value);
    void OnScriptPathChanged(const QString& oldPath, const QString &newPath);
    void OnRemoveClicked();

private:
    bool_input_field *m_triggerStart = nullptr;
    path_input_field *m_scriptPath = nullptr;
    QPushButton *m_removeButton = nullptr;
};

} // namespace editor