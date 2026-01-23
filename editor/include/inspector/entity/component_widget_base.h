//
// Created by gorev on 16.11.2025.
//

#pragma once
#include "scene/Entity.h"
#include "themed_panel.h"


class QLabel;
class QPushButton;
namespace editor
{

class component_widget_base : public themed_panel
{
public:
    explicit component_widget_base(const Blainn::Entity &entity, const QString &name, QWidget *parent = nullptr);

    virtual void OnUpdate();
    virtual void DeleteComponent() = 0;

protected:
    Blainn::Entity m_entity;
    QTimer *m_updateTimer;

    QPushButton *m_settingsButton;
    QMenu *m_settingsMenu;

    QLabel *m_label;

    void ShowSettingsMenu();

    void paintEvent(QPaintEvent *event) override;
};

} // namespace editor