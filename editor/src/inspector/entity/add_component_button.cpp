//
// Created by gorev on 13.11.2025.
//

#include "entity/add_component_button.h"


namespace editor
{
add_component_button::add_component_button(const Blainn::Entity &entity, QWidget *parent)
    : QPushButton(parent)
    , m_entity(entity)
{
    m_menu = new QMenu();

    QAction *transformAction = m_menu->addAction("Transform");

    setText("Add component");

    connect(this, &QPushButton::clicked, this, &add_component_button::OnClicked);
}


void add_component_button::OnClicked()
{
    m_menu->setFixedWidth(width()
                          + 20); // idk why exactly 20, but button width is bigger than setFixedWidth(width()) by 20
    m_menu->popup(mapToGlobal(rect().bottomLeft()));
}
} // namespace editor