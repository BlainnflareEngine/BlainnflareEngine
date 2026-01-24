//
// Created by gorev on 19.01.2026.
//

#include "../../../include/inspector/entity/perception_widget.h"

#include "components/PerceptionComponent.h"
#include "input-widgets/bool_input_field.h"
#include "input-widgets/collapsible_group.h"
#include "input-widgets/float_input_field.h"
#include "input-widgets/string_list_input.h"
#include "scene/EntityTemplates.h"


namespace editor
{
perception_widget::perception_widget(const Blainn::Entity &entity, QWidget *parent)
    : component_widget_base(entity, "Perception", parent)
{
    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::PerceptionComponent>())
    {
        deleteLater();
        return;
    }

    auto comp = m_entity.GetComponent<Blainn::PerceptionComponent>();

    setLayout(new QVBoxLayout(this));

    // Sight Group
    m_sightGroup = new collapsible_group("Sight", THIRD, this);
    m_sightEnabled = new bool_input_field("Enable Sight", comp.enableSight, this);
    m_sightRange = new float_input_field("Range", comp.sightRange, this);
    m_sightFOV = new float_input_field("FOV (degrees)", comp.sightFOV, this);
    m_sightForgetTime = new float_input_field("Forget Time (s)", comp.sightForgetTime, this);
    m_sightLOSCheckInterval = new float_input_field("LOS Check Interval (s)", comp.sightLOSCheckInterval, this);
    m_sightRequireLOS = new bool_input_field("Require Line of Sight", comp.sightRequireLOS, this);

    m_sightRange->SetMinValue(0.1f);
    m_sightFOV->SetMinValue(1.0f);
    m_sightFOV->SetMaxValue(360.0f);
    m_sightForgetTime->SetMinValue(0.1f);
    m_sightLOSCheckInterval->SetMinValue(0.01f);

    m_sightGroup->AddWidget(m_sightEnabled);
    m_sightGroup->AddWidget(m_sightRange);
    m_sightGroup->AddWidget(m_sightFOV);
    m_sightGroup->AddWidget(m_sightForgetTime);
    m_sightGroup->AddWidget(m_sightLOSCheckInterval);
    m_sightGroup->AddWidget(m_sightRequireLOS);

    layout()->addWidget(m_sightGroup);

    // Sound Group
    m_soundGroup = new collapsible_group("Sound", THIRD, this);
    m_soundEnabled = new bool_input_field("Enable Sound", comp.enableSound, this);
    m_soundRange = new float_input_field("Range", comp.soundRange, this);
    m_soundMinStrength = new float_input_field("Min Strength", comp.soundMinStrength, this);
    m_soundForgetTime = new float_input_field("Forget Time (s)", comp.soundForgetTime, this);

    m_soundRange->SetMinValue(0.1f);
    m_soundMinStrength->SetMinValue(0.0f);
    m_soundMinStrength->SetMaxValue(1.0f);
    m_soundForgetTime->SetMinValue(0.1f);

    m_soundGroup->AddWidget(m_soundEnabled);
    m_soundGroup->AddWidget(m_soundRange);
    m_soundGroup->AddWidget(m_soundMinStrength);
    m_soundGroup->AddWidget(m_soundForgetTime);

    layout()->addWidget(m_soundGroup);


    // Touch Group
    m_touchGroup = new collapsible_group("Touch", THIRD, this);
    m_touchEnabled = new bool_input_field("Enable Touch", comp.touchForgetTime, this);
    m_touchForgetTime = new float_input_field("Forget Time (s)", comp.touchForgetTime, this);
    m_touchForgetTime->SetMinValue(0.1f);

    m_touchGroup->AddWidget(m_touchEnabled);
    m_touchGroup->AddWidget(m_touchForgetTime);

    layout()->addWidget(m_touchGroup);

    // Damage Group
    m_damageGroup = new collapsible_group("Damage", THIRD, this);
    m_damageEnabled = new bool_input_field("Enable Damage", comp.enableDamage, this);
    m_damageForgetTime = new float_input_field("Forget Time (s)", comp.damageForgetTime, this);
    m_damageForgetTime->SetMinValue(0.1f);

    m_damageGroup->AddWidget(m_damageEnabled);
    m_damageGroup->AddWidget(m_damageForgetTime);

    layout()->addWidget(m_damageGroup);

    m_ignoreTagsInput = new string_list_input("Ignore Tags", "Enter tag...", comp.ignoreTags, this);
    m_priorityTagsInput = new string_list_input("Priority Tags", "Enter tag...", comp.priorityTags, this);

    layout()->addWidget(m_ignoreTagsInput);
    layout()->addWidget(m_priorityTagsInput);

    connect(m_ignoreTagsInput, &string_list_input::ValueChanged, this, &perception_widget::OnIgnoreTagsChanged);
    connect(m_priorityTagsInput, &string_list_input::ValueChanged, this, &perception_widget::OnPriorityTagsChanged);

    connect(m_damageEnabled, &bool_input_field::toggled, this, &perception_widget::OnDamageEnabledChanged);
    connect(m_damageForgetTime, &float_input_field::EditingFinished, this,
            &perception_widget::OnDamageForgetTimeChanged);

    connect(m_touchEnabled, &bool_input_field::toggled, this, &perception_widget::OnTouchEnabledChanged);
    connect(m_touchForgetTime, &float_input_field::EditingFinished, this, &perception_widget::OnTouchForgetTimeChanged);

    connect(m_soundEnabled, &bool_input_field::toggled, this, &perception_widget::OnSoundEnabledChanged);
    connect(m_soundRange, &float_input_field::EditingFinished, this, &perception_widget::OnSoundRangeChanged);
    connect(m_soundMinStrength, &float_input_field::EditingFinished, this,
            &perception_widget::OnSoundMinStrengthChanged);
    connect(m_soundForgetTime, &float_input_field::EditingFinished, this, &perception_widget::OnSoundForgetTimeChanged);

    connect(m_sightEnabled, &bool_input_field::toggled, this, &perception_widget::OnSightEnabledChanged);
    connect(m_sightRange, &float_input_field::EditingFinished, this, &perception_widget::OnSightRangeChanged);
    connect(m_sightFOV, &float_input_field::EditingFinished, this, &perception_widget::OnSightFOVChanged);
    connect(m_sightForgetTime, &float_input_field::EditingFinished, this, &perception_widget::OnSightForgetTimeChanged);
    connect(m_sightLOSCheckInterval, &float_input_field::EditingFinished, this,
            &perception_widget::OnSightLOSCheckIntervalChanged);
    connect(m_sightRequireLOS, &bool_input_field::toggled, this, &perception_widget::OnSightRequireLOSChanged);
}


void perception_widget::DeleteComponent()
{
    if (m_entity.IsValid()) m_entity.RemoveComponentIfExists<Blainn::PerceptionComponent>();

    deleteLater();
}
void perception_widget::paintEvent(QPaintEvent *event)
{
    BLAINN_PROFILE_FUNC();

    component_widget_base::paintEvent(event);
}


/*void perception_widget::LoadValues()
{
    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::PerceptionComponent>()) return;

    const auto &comp = m_entity.GetComponent<Blainn::PerceptionComponent>();

    // Sight
    m_sightEnabled->setChecked(comp.enableSight);
    m_sightRange->SetValue(comp.sightRange);
    m_sightFOV->SetValue(comp.sightFOV);
    m_sightForgetTime->SetValue(comp.sightForgetTime);
    m_sightLOSCheckInterval->SetValue(comp.sightLOSCheckInterval);
    m_sightRequireLOS->setChecked(comp.sightRequireLOS);

    // Sound
    m_soundEnabled->setChecked(comp.enableSound);
    m_soundRange->SetValue(comp.soundRange);
    m_soundMinStrength->SetValue(comp.soundMinStrength);
    m_soundForgetTime->SetValue(comp.soundForgetTime);

    // Touch
    m_touchEnabled->setChecked(comp.enableTouch);
    m_touchForgetTime->SetValue(comp.touchForgetTime);

    // Damage
    m_damageEnabled->setChecked(comp.enableDamage);
    m_damageForgetTime->SetValue(comp.damageForgetTime);


}*/

void perception_widget::OnSightEnabledChanged()
{
    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::PerceptionComponent>()) return;
    auto &comp = m_entity.GetComponent<Blainn::PerceptionComponent>();
    comp.enableSight = m_sightEnabled->isChecked();
}

void perception_widget::OnSightRangeChanged()
{
    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::PerceptionComponent>()) return;
    auto &comp = m_entity.GetComponent<Blainn::PerceptionComponent>();
    comp.sightRange = m_sightRange->GetValue();
}

void perception_widget::OnSightFOVChanged()
{
    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::PerceptionComponent>()) return;
    auto &comp = m_entity.GetComponent<Blainn::PerceptionComponent>();
    comp.sightFOV = m_sightFOV->GetValue();
}

void perception_widget::OnSightForgetTimeChanged()
{
    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::PerceptionComponent>()) return;
    auto &comp = m_entity.GetComponent<Blainn::PerceptionComponent>();
    comp.sightForgetTime = m_sightForgetTime->GetValue();
}

void perception_widget::OnSightLOSCheckIntervalChanged()
{
    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::PerceptionComponent>()) return;
    auto &comp = m_entity.GetComponent<Blainn::PerceptionComponent>();
    comp.sightLOSCheckInterval = m_sightLOSCheckInterval->GetValue();
}

void perception_widget::OnSightRequireLOSChanged()
{
    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::PerceptionComponent>()) return;
    auto &comp = m_entity.GetComponent<Blainn::PerceptionComponent>();
    comp.sightRequireLOS = m_sightRequireLOS->isChecked();
}

// Sound Slots
void perception_widget::OnSoundEnabledChanged()
{
    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::PerceptionComponent>()) return;
    auto &comp = m_entity.GetComponent<Blainn::PerceptionComponent>();
    comp.enableSound = m_soundEnabled->isChecked();
}

void perception_widget::OnSoundRangeChanged()
{
    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::PerceptionComponent>()) return;
    auto &comp = m_entity.GetComponent<Blainn::PerceptionComponent>();
    comp.soundRange = m_soundRange->GetValue();
}

void perception_widget::OnSoundMinStrengthChanged()
{
    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::PerceptionComponent>()) return;
    auto &comp = m_entity.GetComponent<Blainn::PerceptionComponent>();
    comp.soundMinStrength = m_soundMinStrength->GetValue();
}

void perception_widget::OnSoundForgetTimeChanged()
{
    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::PerceptionComponent>()) return;
    auto &comp = m_entity.GetComponent<Blainn::PerceptionComponent>();
    comp.soundForgetTime = m_soundForgetTime->GetValue();
}

// Touch Slots
void perception_widget::OnTouchEnabledChanged()
{
    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::PerceptionComponent>()) return;
    auto &comp = m_entity.GetComponent<Blainn::PerceptionComponent>();
    comp.enableTouch = m_touchEnabled->isChecked();
}

void perception_widget::OnTouchForgetTimeChanged()
{
    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::PerceptionComponent>()) return;
    auto &comp = m_entity.GetComponent<Blainn::PerceptionComponent>();
    comp.touchForgetTime = m_touchForgetTime->GetValue();
}

// Damage Slots
void perception_widget::OnDamageEnabledChanged()
{
    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::PerceptionComponent>()) return;
    auto &comp = m_entity.GetComponent<Blainn::PerceptionComponent>();
    comp.enableDamage = m_damageEnabled->isChecked();
}

void perception_widget::OnDamageForgetTimeChanged()
{
    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::PerceptionComponent>()) return;
    auto &comp = m_entity.GetComponent<Blainn::PerceptionComponent>();
    comp.damageForgetTime = m_damageForgetTime->GetValue();
}


void perception_widget::OnIgnoreTagsChanged()
{
    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::PerceptionComponent>()) return;
    auto &comp = m_entity.GetComponent<Blainn::PerceptionComponent>();
    comp.ignoreTags = m_ignoreTagsInput->GetValue();
}


void perception_widget::OnPriorityTagsChanged()
{
    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::PerceptionComponent>()) return;
    auto &comp = m_entity.GetComponent<Blainn::PerceptionComponent>();
    comp.priorityTags = m_priorityTagsInput->GetValue();
}
} // namespace editor