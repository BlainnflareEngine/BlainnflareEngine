//
// Created by gorev on 24.01.2026.
//

#include "inspector/entity/light/point_light_widget.h"

#include "components/LightComponent.h"
#include "input-widgets/color_input_field.h"
#include "input-widgets/float_input_field.h"
#include "scene/EntityTemplates.h"
#include <QLayout>

namespace editor
{
point_light_widget::point_light_widget(const Blainn::Entity &entity, QWidget *parent)
    : component_widget_base(entity, "Point light", parent)
{
    Blainn::PointLightComponent *comp;
    if (!m_entity.IsValid() || !(comp = m_entity.TryGetComponent<Blainn::PointLightComponent>()))
    {
        deleteLater();
        return;
    }

    QColor color = {ConvertDXColorToQColor(comp->Color.x), ConvertDXColorToQColor(comp->Color.y),
                    ConvertDXColorToQColor(comp->Color.z), ConvertDXColorToQColor(comp->Color.w)};
    m_color = new color_input_field("Color", color, this);

    m_range = new float_input_field("Range", comp->FalloffEnd, this);
    m_attenuation = new float_input_field("Attenuation", comp->FalloffStart, this);

    layout()->addWidget(m_color);
    layout()->addWidget(m_range);
    layout()->addWidget(m_attenuation);

    connect(m_color, &color_input_field::EditingFinished, this, &point_light_widget::OnColorChanged);
    connect(m_range, &float_input_field::EditingFinished, this, &point_light_widget::OnRangeChanged);
    connect(m_attenuation, &float_input_field::EditingFinished, this, &point_light_widget::OnAttenuationChanged);
}


void point_light_widget::DeleteComponent()
{
    if (m_entity.IsValid()) m_entity.RemoveComponentIfExists<Blainn::PointLightComponent>();

    deleteLater();
}


void point_light_widget::OnUpdate()
{
    component_widget_base::OnUpdate();
}


void point_light_widget::OnColorChanged()
{
    auto light = m_entity.TryGetComponent<Blainn::PointLightComponent>();
    if (!light) deleteLater();

    Blainn::Color newColor = {ConvertQColorToDXColor(m_color->GetValue().red()),
                              ConvertQColorToDXColor(m_color->GetValue().green()),
                              ConvertQColorToDXColor(m_color->GetValue().blue())};
    light->Color = newColor;
    light->MarkFramesDirty();
}


void point_light_widget::OnRangeChanged()
{
    auto light = m_entity.TryGetComponent<Blainn::PointLightComponent>();
    if (!light) deleteLater();

    light->FalloffEnd = m_range->GetValue();
    light->MarkFramesDirty();
}


void point_light_widget::OnAttenuationChanged()
{
    auto light = m_entity.TryGetComponent<Blainn::PointLightComponent>();
    if (!light) deleteLater();

    light->FalloffStart = m_attenuation->GetValue();
    light->MarkFramesDirty();
}
} // namespace editor