//
// Created by gorev on 12.11.2025.
//

#include "entity_inspector_content.h"

#include "../../include/inspector/entity/scripting/scripting_widget.h"
#include "LabelsUtils.h"
#include "components/MeshComponent.h"
#include "components/ScriptingComponent.h"
#include "entity/add_component_button.h"
#include "entity/mesh_widget.h"
#include "entity/transform_widget.h"

namespace editor
{
entity_inspector_content::entity_inspector_content(const EntityInspectorData &data, QWidget *parent)
    : inspector_content_base(parent)
    , m_data(data)
{
    auto entity = m_data.node->GetEntity();
    auto name = m_data.node->GetName();

    if (!entity.IsValid())
    {
        deleteLater();
        return;
    }

    auto boxLayout = new QVBoxLayout();
    setLayout(boxLayout);

    m_tag = new QLabel(ToHeader2(m_data.tag), this);
    m_tag->setTextFormat(Qt::MarkdownText);
    layout()->addWidget(m_tag);

    auto separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    layout()->addWidget(separator);

    connect(m_data.node, &EntityNode::OnTagChanged, this, &entity_inspector_content::SetTag);

    if (entity.HasComponent<Blainn::TransformComponent>())
    {
        auto transform = new transform_widget(m_data.node->GetEntity(), this);
        layout()->addWidget(transform);
    }

    if (entity.HasComponent<Blainn::ScriptingComponent>())
    {
        auto scripting = new scripting_widget(m_data.node->GetEntity(), this);
        layout()->addWidget(scripting);
    }

    if (entity.HasComponent<Blainn::MeshComponent>())
    {
        auto mesh = new mesh_widget(m_data.node->GetEntity(), this);
        layout()->addWidget(mesh);
    }

    auto *addButton = new add_component_button(data.node->GetEntity(),  boxLayout, this);
    layout()->addWidget(addButton);
}


void entity_inspector_content::SetTag(const QString &tag)
{
    m_data.tag = tag;
    m_tag->setText(ToHeader2(tag));
}
} // namespace editor