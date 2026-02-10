//
// Created by gorev on 19.01.2026.
//

#pragma once
#include "component_widget.h"
#include "input-widgets/float_input_field.h"

namespace editor
{
class string_input_field;
class bool_input_field;
} // namespace editor

namespace editor
{

class stimulus_widget : public component_widget<Blainn::TransformComponent>
{
    Q_OBJECT

public:
    explicit stimulus_widget(const Blainn::Entity &entity, QWidget *parent = nullptr);

protected:
    virtual void DeleteComponent() override;
    void paintEvent(QPaintEvent *event) override;
private slots:
    void OnSightChanged(bool value);
    void OnSoundChanged(bool value);
    void OnTouchChanged(bool value);
    void OnDamageChanged(bool value);

    void OnSightRadiusChanged();
    void OnSoundRadiusChanged();
    void OnTagChanged(const QString &tag);

private:
    bool_input_field *m_sight = nullptr;
    bool_input_field *m_sound = nullptr;
    bool_input_field *m_touch = nullptr;
    bool_input_field *m_damage = nullptr;

    float_input_field *m_sightRadius = nullptr;
    float_input_field *m_soundRadius = nullptr;
    string_input_field *m_tag = nullptr;
};

} // namespace editor