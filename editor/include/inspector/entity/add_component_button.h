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
    add_component_button(const Blainn::Entity &entity, QWidget *parent = nullptr);

public slots:
    void OnClicked();


private:
    Blainn::Entity m_entity;

    QMenu *m_menu = nullptr;
};

} // namespace editor