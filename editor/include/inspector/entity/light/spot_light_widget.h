//
// Created by gorev on 24.01.2026.
//

#pragma once
#include "entity/component_widget_base.h"

namespace editor
{
class color_input_field;
}
namespace editor
{
class float_input_field;
}
namespace editor
{

class spot_light_widget : public component_widget_base
{
public:
    explicit spot_light_widget(const Blainn::Entity &entity, QWidget *parent = nullptr);

protected:
    void DeleteComponent() override;
    void OnUpdate() override;

    void OnColorChanged();
    void OnRangeChanged();
    void OnAttenuationChanged();
    void OnIntensityChanged();
    void OnInnerAngleChanged();
    void OnOuterAngleChanged();

private:
    color_input_field *m_color;
    float_input_field *m_intensity;
    float_input_field *m_range;
    float_input_field *m_attenuation;
    float_input_field *m_innerAngle;
    float_input_field *m_outerAngle;
};

} // namespace editor