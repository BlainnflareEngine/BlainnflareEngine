//
// Created by gorev on 19.01.2026.
//

#pragma once
#include "component_widget_base.h"

namespace editor
{
class string_list_input;
class bool_input_field;
} // namespace editor
namespace editor
{
class float_input_field;
class collapsible_group;

class perception_widget : public component_widget_base
{
public:
    explicit perception_widget(const Blainn::Entity &entity, QWidget *parent = nullptr);

protected:
    void DeleteComponent() override;

protected slots:
    void OnSightEnabledChanged();
    void OnSightRangeChanged();
    void OnSightFOVChanged();
    void OnSightForgetTimeChanged();
    void OnSightLOSCheckIntervalChanged();
    void OnSightRequireLOSChanged();
    void OnSoundEnabledChanged();
    void OnSoundRangeChanged();
    void OnSoundMinStrengthChanged();
    void OnSoundForgetTimeChanged();
    void OnTouchEnabledChanged();
    void OnTouchForgetTimeChanged();
    void OnDamageEnabledChanged();
    void OnDamageForgetTimeChanged();
    void OnIgnoreTagsChanged();
    void OnPriorityTagsChanged();

private:
    collapsible_group *m_sightGroup = nullptr;
    bool_input_field *m_sightEnabled = nullptr;
    float_input_field *m_sightRange = nullptr;
    float_input_field *m_sightFOV = nullptr;
    float_input_field *m_sightForgetTime = nullptr;
    float_input_field *m_sightLOSCheckInterval = nullptr;
    bool_input_field *m_sightRequireLOS = nullptr;

    collapsible_group *m_soundGroup = nullptr;
    bool_input_field *m_soundEnabled = nullptr;
    float_input_field *m_soundRange = nullptr;
    float_input_field *m_soundMinStrength = nullptr;
    float_input_field *m_soundForgetTime = nullptr;

    collapsible_group *m_touchGroup = nullptr;
    bool_input_field *m_touchEnabled = nullptr;
    float_input_field *m_touchForgetTime = nullptr;


    collapsible_group *m_damageGroup = nullptr;
    bool_input_field *m_damageEnabled = nullptr;
    float_input_field *m_damageForgetTime = nullptr;

    string_list_input *m_ignoreTagsInput = nullptr;
    string_list_input *m_priorityTagsInput = nullptr;
};

} // namespace editor