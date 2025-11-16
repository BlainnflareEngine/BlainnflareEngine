//
// Created by gorev on 13.11.2025.
//

#include "entity/add_component_button.h"

#include "entity/transform_widget.h"
#include "scene/EntityTemplates.h"

#include <QLayout>


namespace editor
{
add_component_button::add_component_button(const Blainn::Entity &entity, QLayout *layout, QWidget *parent)
    : QPushButton(parent)
    , m_entity(entity)
    , m_layout(layout)
{
    m_menu = new QMenu();

    m_transformAction = m_menu->addAction("Transform");

    setText("Add component");

    connect(this, &QPushButton::clicked, this, &add_component_button::OnClicked);

    connect(m_transformAction, &QAction::triggered, this, &add_component_button::OnTransformAction);
}


void add_component_button::OnClicked()
{
    m_menu->setFixedWidth(width() + 20);
    m_menu->popup(mapToGlobal(rect().bottomLeft()));
}


void add_component_button::OnTransformAction()
{
    if (!m_entity.IsValid()) return;
    if (m_entity.HasComponent<Blainn::TransformComponent>()) return;

    m_entity.AddComponent<Blainn::TransformComponent>();
    auto transform = new transform_widget(m_entity, this);
    m_layout->addWidget(transform);
}
} // namespace editor