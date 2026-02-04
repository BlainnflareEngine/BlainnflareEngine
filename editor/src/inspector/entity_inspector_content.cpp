//
// Created by gorev on 12.11.2025.
//

#include "entity_inspector_content.h"

#include "EditorRegistry.h"
#include "entity/scripting/scripting_widget.h"
#include "LabelsUtils.h"
#include "../../include/inspector/entity/perception_widget.h"
#include "components/AIControllerComponent.h"
#include "components/CameraComponent.h"
#include "components/LightComponent.h"
#include "components/MeshComponent.h"
#include "components/NavMeshVolumeComponent.h"
#include "components/SkyboxComponent.h"
#include "components/PhysicsComponent.h"
#include "components/ScriptingComponent.h"
#include "components/StimulusComponent.h"
#include "entity/add_component_button.h"
#include "entity/ai_controller_widget.h"
#include "entity/camera_widget.h"
#include "../../include/inspector/entity/light/directional_light_widget.h"
#include "entity/mesh_widget.h"
#include "entity/navmesh_volume_widget.h"
#include "entity/physics_widget.h"
#include "entity/transform_widget.h"
#include "entity/skybox_widget.h"
#include "entity/stimulus_widget.h"
#include "entity/light/point_light_widget.h"
#include "entity/light/spot_light_widget.h"

#include <QScrollArea>

namespace editor
{
entity_inspector_content::entity_inspector_content(const Blainn::uuid &id, QWidget *parent)
    : inspector_content_base(parent)
    , m_id(id)
{
    BLAINN_PROFILE_FUNC();

    auto entity = Blainn::Engine::GetSceneManager().TryGetEntityWithUUID(m_id);
    if (!entity.IsValid())
    {
        deleteLater();
        return;
    }

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(5);

    m_tag = new QLabel(ToHeader2(entity.Name().c_str()), this);
    m_tag->setTextFormat(Qt::MarkdownText);
    m_tag->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    mainLayout->addWidget(m_tag);

    auto separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    separator->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    mainLayout->addWidget(separator);

    auto addButton = new add_component_button(entity, mainLayout, this);
    addButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    mainLayout->addWidget(addButton);

    // TODO: tag changed signal
    // connect(m_data.node, &EntityNode::OnTagChanged, this, &entity_inspector_content::SetTag);

    for (const auto &[typeId, factory] : g_widgetRegistry)
    {
        BLAINN_PROFILE_FUNC(typeId);

        if (entity.HasComponent(typeId))
        {
            auto *widget = factory(entity, this);
            mainLayout->addWidget(widget);
        }
    }

    mainLayout->addStretch(1);
}


void entity_inspector_content::SetTag(const QString &tag)
{
    auto entity = Blainn::Engine::GetSceneManager().TryGetEntityWithUUID(m_id);
    if (!entity.IsValid()) return;

    entity.GetComponent<Blainn::TagComponent>().Tag = ToEASTLString(tag);
    m_tag->setText(ToHeader2(tag));
}


void entity_inspector_content::paintEvent(QPaintEvent *event)
{
    BLAINN_PROFILE_FUNC();

    inspector_content_base::paintEvent(event);
}


void entity_inspector_content::resizeEvent(QResizeEvent *event)
{
    BLAINN_PROFILE_FUNC();

    inspector_content_base::resizeEvent(event);
}
} // namespace editor