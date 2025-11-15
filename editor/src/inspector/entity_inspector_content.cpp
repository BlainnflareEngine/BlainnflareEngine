//
// Created by gorev on 12.11.2025.
//

#include "entity_inspector_content.h"

#include "LabelsUtils.h"
#include "entity/add_component_button.h"
#include "entity/transform_widget.h"

namespace editor
{
entity_inspector_content::entity_inspector_content(const EntityInspectorData &data, QWidget *parent)
    : inspector_content_base(parent)
    , m_data(data)
{
    setLayout(new QVBoxLayout());

    m_tag = new QLabel(ToHeader2(m_data.tag), this);
    m_tag->setTextFormat(Qt::MarkdownText);
    layout()->addWidget(m_tag);

    auto separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    layout()->addWidget(separator);

    auto *addButton = new add_component_button(data.node->GetEntity(), layout(), this);
    layout()->addWidget(addButton);

    connect(m_data.node, &EntityNode::OnTagChanged, this, &entity_inspector_content::SetTag);
    // TODO: get all components of this entity and create component widget for each

    if (m_data.node->GetEntity().HasComponent<Blainn::TransformComponent>())
    {
        auto transform = new transform_widget(m_data.node->GetEntity(), this);
        layout()->addWidget(transform);
        // TODO: connect
    }
}


void entity_inspector_content::SetTag(const QString &tag)
{
    m_data.tag = tag;
    m_tag->setText(ToHeader2(tag));
}
} // namespace editor