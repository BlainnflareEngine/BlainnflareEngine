//
// Created by gorev on 13.11.2025.
//

#pragma once
#include "scene/Entity.h"


#include <QMenu>
#include <QPushButton>


namespace editor
{

class add_component_button : public QPushButton
{
    Q_OBJECT
public:
    add_component_button(const Blainn::Entity &entity, QLayout *layout, QWidget *parent);

public slots:
    void OnClicked();
    void OnTransformAction();
    void OnMeshAction();
    void OnScriptingAction();

private:
    Blainn::Entity m_entity;
    QLayout *m_layout;
    QMenu *m_menu = nullptr;
    QAction *m_transformAction = nullptr;
    QAction *m_meshAction = nullptr;
    QAction *m_scriptingAction = nullptr;
};

} // namespace editor