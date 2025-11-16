//
// Created by gorev on 14.11.2025.
//

#pragma once
#include "scene/Entity.h"
#include "themed_panel.h"


#include <QFrame>


namespace editor
{
class float_input_field;
}
class QLineEdit;
class QLabel;
namespace editor
{

class transform_widget : public themed_panel
{
    Q_OBJECT

public:
    explicit transform_widget(const Blainn::Entity &entity, QWidget *parent = nullptr);

private slots:
    void OnPositionChanged();
    void OnRotationChanged();
    void OnScaleChanged();


private:
    Blainn::Entity m_entity;

    QLabel *m_label;

    float_input_field *m_positionX = nullptr;
    float_input_field *m_positionY = nullptr;
    float_input_field *m_positionZ = nullptr;

    float_input_field *m_rotationX = nullptr;
    float_input_field *m_rotationY = nullptr;
    float_input_field *m_rotationZ = nullptr;

    float_input_field *m_scaleX = nullptr;
    float_input_field *m_scaleY = nullptr;
    float_input_field *m_scaleZ = nullptr;

    void CreateTransformFields();
    void LoadTransformValues();
    void ConnectSignals();
    void BlockSignals(bool block);
    static bool IsValidNumericInput(const QString &currentText, const QString &newText);

    QWidget *CreateVector3(const QString &title, float_input_field *&xField, float_input_field *&yField,
                           float_input_field *&zField);
};

} // namespace editor