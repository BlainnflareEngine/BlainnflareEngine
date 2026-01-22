//
// Created by gorev on 22.01.2026.
//

#pragma once
#include "component_widget_base.h"

namespace editor
{
class color_input_field;
}
namespace editor
{

class directional_light_widget : public component_widget_base
{
public:
    explicit directional_light_widget(const Blainn::Entity &entity, QWidget *parent = nullptr);

protected:
    virtual void DeleteComponent() override;
    virtual void OnUpdate() override;

    void OnColorChanged();

private:
    color_input_field *m_lightColor = nullptr;
};

} // namespace editor