//
// Created by gorev on 07.12.2025.
//

#pragma once
#include "component_widget_base.h"
#include "input-widgets/enum_input_widget.h"
#include "input-widgets/vector3_bool_widget.h"

namespace editor
{
class vector3_bool_widget;
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

    void OnPositionConstraintsChanged(const BoolVector3 &value);
    void OnRotationConstraintsChanged(const BoolVector3 &value);

private:
    struct ShapeDimensions
    {
        float radius = 0.0f;
        float halfHeight = 0.0f;
    };

    bool_input_field *m_isTrigger = nullptr;

    float_input_field *m_gravityFactor = nullptr;

    enum_input_widget<Blainn::ComponentShapeType> *m_shape = nullptr;
    enum_input_widget<Blainn::PhysicsComponentMotionType> *m_objectType = nullptr;
    enum_input_widget<Blainn::ObjectLayer> *m_objectLayer = nullptr;

    float_input_field *m_radius = nullptr;
    float_input_field *m_halfHeight = nullptr;
    vector3_input_widget *m_extents = nullptr;

    vector3_bool_widget *m_positionConstraints = nullptr;
    vector3_bool_widget *m_rotationConstraints = nullptr;

    void ShowSphereSettings(float radius);
    void ShowBoxSettings(const Blainn::Vec3 &extents);
    void ShowCylinderSettings(const ShapeDimensions &dimensions);
    void ShowCapsuleSettings(const ShapeDimensions &dimensions);
    void ClearSettings();

    void LoadValues();
    void LoadShape();
    void BlockSignals(bool value);

    void paintEvent(QPaintEvent *event) override;
};

} // namespace editor
