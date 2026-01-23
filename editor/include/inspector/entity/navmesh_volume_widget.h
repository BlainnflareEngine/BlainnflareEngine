#pragma once

#include "component_widget_base.h"


namespace editor
{
class vector3_input_widget;

class navmesh_volume_widget : public component_widget_base
{
public:
    explicit navmesh_volume_widget(const Blainn::Entity &entity, QWidget *parent = nullptr);

protected:
    virtual void DeleteComponent() override;
    void OnUpdate() override;
    void paintEvent(QPaintEvent *event) override;

private:
    vector3_input_widget *m_extents = nullptr;

    void OnExtentsChanged();
};
} // namespace editor