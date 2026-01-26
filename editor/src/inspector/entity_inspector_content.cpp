//
// Created by gorev on 12.11.2025.
//

#include "entity_inspector_content.h"

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
entity_inspector_content::entity_inspector_content(const EntityInspectorData &data, QWidget *parent)
    : inspector_content_base(parent)
    , m_data(data)
{
    BLAINN_PROFILE_FUNC();

    auto entity = m_data.node->GetEntity();
    if (!entity.IsValid())
    {
        deleteLater();
        return;
    }

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(5);

    m_tag = new QLabel(ToHeader2(m_data.tag), this);
    m_tag->setTextFormat(Qt::MarkdownText);
    mainLayout->addWidget(m_tag);

    auto separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    mainLayout->addWidget(separator);

    auto addButton = new add_component_button(entity, mainLayout, this);
    mainLayout->addWidget(addButton);

    connect(m_data.node, &EntityNode::OnTagChanged, this, &entity_inspector_content::SetTag);

    if (entity.HasComponent<Blainn::TransformComponent>())
    {
        BLAINN_PROFILE_SCOPE(CreateTransformWidget);

        auto transform = new transform_widget(m_data.node->GetEntity(), this);
        layout()->addWidget(transform);
    }

    if (entity.HasComponent<Blainn::DirectionalLightComponent>())
    {
        BLAINN_PROFILE_SCOPE(CreateDirectionalLightWidget);

        auto dirLight = new directional_light_widget(m_data.node->GetEntity(), this);
        layout()->addWidget(dirLight);
    }

    if (entity.HasComponent<Blainn::PointLightComponent>())
    {
        BLAINN_PROFILE_SCOPE(CreatePointLightWidget);

        auto dirLight = new point_light_widget(m_data.node->GetEntity(), this);
        layout()->addWidget(dirLight);
    }

    if (entity.HasComponent<Blainn::SpotLightComponent>())
    {
        BLAINN_PROFILE_SCOPE(CreateSpotLightWidget);

        auto dirLight = new spot_light_widget(m_data.node->GetEntity(), this);
        layout()->addWidget(dirLight);
    }

    if (entity.HasComponent<Blainn::PhysicsComponent>())
    {
        BLAINN_PROFILE_SCOPE(CreatePhysicsWidget);

        auto physics = new physics_widget(m_data.node->GetEntity(), this);
        layout()->addWidget(physics);
    }

    if (entity.HasComponent<Blainn::ScriptingComponent>())
    {
        BLAINN_PROFILE_SCOPE(CreateScriptingWidget);

        auto scripting = new scripting_widget(m_data.node->GetEntity(), this);
        layout()->addWidget(scripting);
    }

    if (entity.HasComponent<Blainn::AIControllerComponent>())
    {
        BLAINN_PROFILE_SCOPE(CreateAIControllerWidget);

        auto aiController = new ai_controller_widget(m_data.node->GetEntity(), this);
        layout()->addWidget(aiController);
    }

    if (entity.HasComponent<Blainn::StimulusComponent>())
    {
        BLAINN_PROFILE_SCOPE(CreateStimulusWidget);

        auto stimulus = new stimulus_widget(m_data.node->GetEntity(), this);
        layout()->addWidget(stimulus);
    }

    if (entity.HasComponent<Blainn::PerceptionComponent>())
    {
        BLAINN_PROFILE_SCOPE(CreatePerceptionWidget);

        auto perception = new perception_widget(m_data.node->GetEntity(), this);
        layout()->addWidget(perception);
    }

    if (entity.HasComponent<Blainn::MeshComponent>())
    {
        BLAINN_PROFILE_SCOPE(CreateMeshWidget);

        auto mesh = new mesh_widget(m_data.node->GetEntity(), this);
        layout()->addWidget(mesh);
    }

    if (entity.HasComponent<Blainn::SkyboxComponent>())
    {
        BLAINN_PROFILE_SCOPE(CreateSkyboxWidget);

        auto skybox = new skybox_widget(m_data.node->GetEntity(), this);
        layout()->addWidget(skybox);
    }

    if (entity.HasComponent<Blainn::CameraComponent>())
    {
        BLAINN_PROFILE_SCOPE(CreateCameraWidget);

        auto camera = new camera_widget(m_data.node->GetEntity(), this);
        layout()->addWidget(camera);
    }

    if (entity.HasComponent<Blainn::NavmeshVolumeComponent>())
    {
        BLAINN_PROFILE_SCOPE(CreateNavmeshVolumeWidget);

        auto navmeshVolume = new navmesh_volume_widget(m_data.node->GetEntity(), this);
        layout()->addWidget(navmeshVolume);
    }

    {
        BLAINN_PROFILE_SCOPE(AddStretch);
        mainLayout->addStretch(1);
    }
}


void entity_inspector_content::SetTag(const QString &tag)
{
    m_data.tag = tag;
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