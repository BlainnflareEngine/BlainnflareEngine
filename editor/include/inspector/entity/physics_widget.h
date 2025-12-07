//
// Created by gorev on 07.12.2025.
//

#pragma once
#include "component_widget_base.h"
#include "input-widgets/enum_input_widget.h"

namespace editor
{
class vector3_input_widget;
class float_input_field;
} // namespace editor
namespace editor
{
class bool_input_field;
}
namespace editor
{

class physics_widget : public component_widget_base
{
public:
    explicit physics_widget(const Blainn::Entity &entity, QWidget *parent = nullptr);

    void DeleteComponent() override;

protected slots:
    void OnShapeChanged(int);
    void OnObjectLayerChanged(int);
    void OnObjectTypeChanged(int);

    void OnGravityChanged();
    void OnTriggerChanged();
    void OnRadiusChanged();
    void OnHalfHeightChanged();
    void OnExtentsChanged();

private:
    bool_input_field *m_isTrigger = nullptr;

    float_input_field *m_gravityFactor = nullptr;

    enum_input_widget<Blainn::ComponentShapeType> *m_shape = nullptr;
    enum_input_widget<Blainn::PhysicsComponentMotionType> *m_objectType = nullptr;
    enum_input_widget<Blainn::ObjectLayer> *m_objectLayer = nullptr;

    float_input_field *m_radius = nullptr;
    float_input_field *m_halfHeight = nullptr;
    vector3_input_widget *m_extents = nullptr;

    void ShowSphereSettings();
    void ShowBoxSettings();
    void ShowCylinderSettings();
    void ShowCapsuleSettings();
    void ClearSettings();
};

} // namespace editor