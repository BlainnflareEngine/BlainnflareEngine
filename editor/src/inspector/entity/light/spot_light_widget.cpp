//
// Created by gorev on 24.01.2026.
//

#include "entity/light/spot_light_widget.h"
#include "components/LightComponent.h"
#include "input-widgets/color_input_field.h"
#include "input-widgets/float_input_field.h"
#include "scene/EntityTemplates.h"
#include <QLayout>

namespace editor
{
spot_light_widget::spot_light_widget(const Blainn::Entity &entity, QWidget *parent)
    : component_widget_base(entity, "Spot light", parent)
{
    Blainn::SpotLightComponent *comp;
    if (!m_entity.IsValid() || !(comp = m_entity.TryGetComponent<Blainn::SpotLightComponent>()))
    {
        deleteLater();
        return;
    }

    QColor color = {ConvertDXColorToQColor(comp->Color.x), ConvertDXColorToQColor(comp->Color.y),
                    ConvertDXColorToQColor(comp->Color.z), ConvertDXColorToQColor(comp->Color.w)};
    m_color = new color_input_field("Color", color, this);

    m_intensity = new float_input_field("Intensity", comp->Intensity, this);
    m_intensity->SetMinValue(0);

    m_range = new float_input_field("Range", comp->FalloffEnd, this);
    m_range->SetMinValue(0);

    m_attenuation = new float_input_field("Attenuation", comp->FalloffStart, this);
    m_attenuation->SetMinValue(0);

    m_innerAngle = new float_input_field("Inner angle", comp->SpotInnerAngle, this);
    m_innerAngle->SetMinValue(0);

    m_outerAngle = new float_input_field("Outer angle", comp->SpotOuterAngle, this);
    m_outerAngle->SetMinValue(0);

    layout()->addWidget(m_color);
    layout()->addWidget(m_intensity);
    layout()->addWidget(m_range);
    layout()->addWidget(m_attenuation);
    layout()->addWidget(m_innerAngle);
    layout()->addWidget(m_outerAngle);

    connect(m_color, &color_input_field::EditingFinished, this, &spot_light_widget::OnColorChanged);
    connect(m_range, &float_input_field::EditingFinished, this, &spot_light_widget::OnRangeChanged);
    connect(m_attenuation, &float_input_field::EditingFinished, this, &spot_light_widget::OnAttenuationChanged);
    connect(m_intensity, &float_input_field::EditingFinished, this, &spot_light_widget::OnIntensityChanged);
    connect(m_innerAngle, &float_input_field::EditingFinished, this, &spot_light_widget::OnInnerAngleChanged);
    connect(m_outerAngle, &float_input_field::EditingFinished, this, &spot_light_widget::OnOuterAngleChanged);
}


void spot_light_widget::DeleteComponent()
{
    if (m_entity.IsValid()) m_entity.RemoveComponentIfExists<Blainn::SpotLightComponent>();

    deleteLater();
}


void spot_light_widget::OnUpdate()
{
    component_widget_base::OnUpdate();
}


void spot_light_widget::OnColorChanged()
{
    auto light = m_entity.TryGetComponent<Blainn::SpotLightComponent>();
    if (!light) deleteLater();

    Blainn::Color newColor = {ConvertQColorToDXColor(m_color->GetValue().red()),
                              ConvertQColorToDXColor(m_color->GetValue().green()),
                              ConvertQColorToDXColor(m_color->GetValue().blue())};
    light->Color = newColor;
    light->MarkFramesDirty();
}


void spot_light_widget::OnRangeChanged()
{
    auto light = m_entity.TryGetComponent<Blainn::SpotLightComponent>();
    if (!light) deleteLater();

    light->FalloffEnd = m_range->GetValue();

    if (light->FalloffStart > light->FalloffEnd) m_attenuation->SetValue(light->FalloffEnd - 0.001);

    light->MarkFramesDirty();
}


void spot_light_widget::OnAttenuationChanged()
{
    auto light = m_entity.TryGetComponent<Blainn::SpotLightComponent>();
    if (!light) deleteLater();

    light->FalloffStart = m_attenuation->GetValue();

    if (light->FalloffStart > light->FalloffEnd) m_attenuation->SetValue(light->FalloffEnd - 0.001);

    light->MarkFramesDirty();
}


void spot_light_widget::OnIntensityChanged()
{
    auto light = m_entity.TryGetComponent<Blainn::SpotLightComponent>();
    if (!light) deleteLater();

    light->Intensity = m_intensity->GetValue();
    light->MarkFramesDirty();
}


void spot_light_widget::OnInnerAngleChanged()
{
}


void spot_light_widget::OnOuterAngleChanged()
{
}
} // namespace editor