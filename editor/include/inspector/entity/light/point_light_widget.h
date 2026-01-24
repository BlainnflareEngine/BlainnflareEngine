//
// Created by gorev on 24.01.2026.
//

#pragma once
#include "../component_widget_base.h"

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

class point_light_widget : public component_widget_base
{
public:
    explicit point_light_widget(const Blainn::Entity &entity, QWidget *parent = nullptr);

protected:
    void DeleteComponent() override;
    void OnUpdate() override;

    void OnColorChanged(const QColor &color);
    void OnRangeChanged();
    void OnAttenuationChanged();

private:
    color_input_field *m_color;
    float_input_field *m_range;
    float_input_field *m_attenuation;
};

} // namespace editor