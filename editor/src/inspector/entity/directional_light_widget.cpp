//
// Created by gorev on 22.01.2026.
//

#include "../../../include/inspector/entity/directional_light_widget.h"

#include "Components/LightComponent.h"
#include "Render/DebugRenderer.h"
#include "input-widgets/color_input_field.h"
#include "scene/EntityTemplates.h"
#include "scene/BasicComponents.h"

#include <QLayout>

namespace editor
{
directional_light_widget::directional_light_widget(const Blainn::Entity &entity, QWidget *parent)
    : component_widget_base(entity, "Directional light", parent)
{
    auto light = m_entity.TryGetComponent<Blainn::DirectionalLightComponent>();
    if (!light) deleteLater();

    QColor color = {ConvertDXColorToQColor(light->Color.x), ConvertDXColorToQColor(light->Color.y),
                    ConvertDXColorToQColor(light->Color.z), ConvertDXColorToQColor(light->Color.w)};
    m_lightColor = new color_input_field("Light color", color, this);
    layout()->addWidget(m_lightColor);

    connect(m_lightColor, &color_input_field::EditingFinished, this, &directional_light_widget::OnColorChanged);
}

void directional_light_widget::DeleteComponent()
{
    if (auto component = m_entity.TryGetComponent<Blainn::DirectionalLightComponent>())
        m_entity.RemoveComponent<Blainn::DirectionalLightComponent>();

    deleteLater();
}

void directional_light_widget::OnUpdate()
{
    using namespace Blainn;

    component_widget_base::OnUpdate();

    if (!RenderSubsystem::GetInstance().DebugEnabled()) return;

    auto transform = m_entity.TryGetComponent<TransformComponent>();
    if (!transform) return;

    Vec3 from = transform->GetTranslation();
    Vec3 to = from + transform->GetForwardVector();

    RenderSubsystem::GetInstance().GetDebugRenderer().DrawArrow(from, to, {1, 1, 0, 1}, 0.2);
}


void directional_light_widget::OnColorChanged()
{
    auto light = m_entity.TryGetComponent<Blainn::DirectionalLightComponent>();
    if (!light) deleteLater();

    Blainn::Color color = {ConvertQColorToDXColor(m_lightColor->GetValue().red()),
                           ConvertQColorToDXColor(m_lightColor->GetValue().green()),
                           ConvertQColorToDXColor(m_lightColor->GetValue().blue())};
    light->Color = color;
}


void directional_light_widget::paintEvent(QPaintEvent *event)
{
    BLAINN_PROFILE_FUNC();

    component_widget_base::paintEvent(event);
}
} // namespace editor