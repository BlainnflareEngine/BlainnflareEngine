//
// Created by gorev on 14.11.2025.
//

#pragma once
#include "component_widget_base.h"
#include "scene/Entity.h"


#include <QFrame>


namespace editor
{
class vector3_input_widget;
}
namespace editor
{
class float_input_field;
}
class QLineEdit;
class QLabel;
namespace editor
{

class transform_widget : public component_widget_base
{
    Q_OBJECT

public:
    explicit transform_widget(const Blainn::Entity &entity, QWidget *parent = nullptr);

protected slots:
    void OnPositionChanged();
    void OnRotationChanged();
    void OnScaleChanged();

protected:
    virtual void OnUpdate() override;
    virtual void DeleteComponent() override;
    void paintEvent(QPaintEvent *event) override;

private:
    vector3_input_widget *m_position;
    vector3_input_widget *m_rotation;
    vector3_input_widget *m_scale;

    void CreateTransformFields();
    void LoadTransformValues();
    void ConnectSignals();
    void BlockSignals(bool block);
    static bool IsValidNumericInput(const QString &newText);

    QWidget *CreateVector3(const QString &title, float_input_field *&xField, float_input_field *&yField,
                           float_input_field *&zField);
};

} // namespace editor
