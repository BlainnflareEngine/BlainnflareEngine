//
// Created by gorev on 14.01.2026.
//

#pragma once
#include "component_widget_base.h"

namespace editor
{
class bool_input_field;
}
namespace editor
{
class float_input_field;
class path_input_field;

class ai_controller_widget : public component_widget_base
{
public:
    explicit ai_controller_widget(const Blainn::Entity &entity, QWidget *parent = nullptr);

protected:
    virtual void DeleteComponent() override;
    virtual void OnUpdate() override;

protected slots:
    void OnPathChanged(const QString &old, const QString &path);
    void OnMovementSpeedChanged();
    void OnStoppingDistanceChanged();
    void OnGroundOffsetChanged();
    void OnFaceMovementDirectionChanged(bool value);
    void OnRotationSpeedChanged();

    path_input_field *m_path_input = nullptr;
    float_input_field *m_movementSpeed = nullptr;
    float_input_field *m_stoppingDistance = nullptr;
    float_input_field *m_groundOffset = nullptr;
    bool_input_field* m_faceDirection = nullptr;
    float_input_field *m_rotationSpeed = nullptr;

private:
    void paintEvent(QPaintEvent *event) override;
};

} // namespace editor