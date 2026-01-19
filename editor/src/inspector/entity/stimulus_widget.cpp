//
// Created by gorev on 19.01.2026.
//

#include "entity/stimulus_widget.h"

#include "FileSystemUtils.h"
#include "LabelsUtils.h"
#include "components/StimulusComponent.h"
#include "input-widgets/bool_input_field.h"
#include "input-widgets/string_input_field.h"
#include "oclero/qlementine/widgets/Label.hpp"
#include "scene/EntityTemplates.h"

#include <QLayout>

namespace editor
{
stimulus_widget::stimulus_widget(const Blainn::Entity &entity, QWidget *parent)
    : component_widget_base(entity, "Stimulus", parent)
{
    auto comp = entity.TryGetComponent<Blainn::StimulusComponent>();
    if (!comp)
    {
        deleteLater();
        return;
    }

    auto labelReactions = new QLabel(ToHeader2("Event reactions"), this);
    labelReactions->setTextFormat(Qt::MarkdownText);
    auto separatorReactions = new QFrame(this);
    separatorReactions->setFrameStyle(HLine);

    m_sight = new bool_input_field("Sight", comp->enableSight, this);
    m_sound = new bool_input_field("Sound", comp->enableSound, this);
    m_touch = new bool_input_field("Touch", comp->enableTouch, this);
    m_damage = new bool_input_field("Damage", comp->enableDamage, this);

    auto labelParameters = new QLabel(ToHeader2("Parameters"), this);
    labelParameters->setTextFormat(Qt::MarkdownText);
    auto separatorParameters = new QFrame(this);
    separatorParameters->setFrameStyle(HLine);

    m_sightRadius = new float_input_field("Sight radius", comp->sightRadius, this);
    m_soundRadius = new float_input_field("Sound radius", comp->soundRadius, this);
    m_tag = new string_input_field("Tag", comp->tag.c_str(), "No tag", this);

    layout()->addWidget(labelReactions);
    layout()->addWidget(separatorReactions);
    layout()->addWidget(m_sight);
    layout()->addWidget(m_sound);
    layout()->addWidget(m_touch);
    layout()->addWidget(m_damage);
    layout()->addWidget(labelParameters);
    layout()->addWidget(separatorParameters);
    layout()->addWidget(m_sightRadius);
    layout()->addWidget(m_soundRadius);
    layout()->addWidget(m_tag);

    connect(m_sight, &bool_input_field::toggled, this, &stimulus_widget::OnSightChanged);
    connect(m_sound, &bool_input_field::toggled, this, &stimulus_widget::OnSoundChanged);
    connect(m_touch, &bool_input_field::toggled, this, &stimulus_widget::OnTouchChanged);
    connect(m_damage, &bool_input_field::toggled, this, &stimulus_widget::OnDamageChanged);
    connect(m_sightRadius, &float_input_field::EditingFinished, this, &stimulus_widget::OnSightRadiusChanged);
    connect(m_soundRadius, &float_input_field::EditingFinished, this, &stimulus_widget::OnSoundRadiusChanged);
    connect(m_tag, &string_input_field::EditingFinished, this, &stimulus_widget::OnTagChanged);
}


void stimulus_widget::DeleteComponent()
{
    if (m_entity.IsValid()) m_entity.RemoveComponentIfExists<Blainn::StimulusComponent>();
    deleteLater();
}


void stimulus_widget::OnSightChanged(bool value)
{
    if (!m_entity.IsValid())
    {
        deleteLater();
        return;
    }

    if (auto comp = m_entity.TryGetComponent<Blainn::StimulusComponent>())
    {
        comp->enableSight = value;
    }
}

void stimulus_widget::OnSoundChanged(bool value)
{
    if (!m_entity.IsValid())
    {
        deleteLater();
        return;
    }

    if (auto comp = m_entity.TryGetComponent<Blainn::StimulusComponent>())
    {
        comp->enableSound = value;
    }
}


void stimulus_widget::OnTouchChanged(bool value)
{
    if (!m_entity.IsValid())
    {
        deleteLater();
        return;
    }

    if (auto comp = m_entity.TryGetComponent<Blainn::StimulusComponent>())
    {
        comp->enableTouch = value;
    }
}


void stimulus_widget::OnDamageChanged(bool value)
{
    if (!m_entity.IsValid())
    {
        deleteLater();
        return;
    }

    if (auto comp = m_entity.TryGetComponent<Blainn::StimulusComponent>())
    {
        comp->enableDamage = value;
    }
}


void stimulus_widget::OnSightRadiusChanged()
{
    if (!m_entity.IsValid())
    {
        deleteLater();
        return;
    }

    if (auto comp = m_entity.TryGetComponent<Blainn::StimulusComponent>())
    {
        comp->sightRadius = m_sightRadius->GetValue();
    }
}


void stimulus_widget::OnSoundRadiusChanged()
{
    if (!m_entity.IsValid())
    {
        deleteLater();
        return;
    }

    if (auto comp = m_entity.TryGetComponent<Blainn::StimulusComponent>())
    {
        comp->soundRadius = m_soundRadius->GetValue();
    }
}


void stimulus_widget::OnTagChanged(const QString &tag)
{
    if (!m_entity.IsValid())
    {
        deleteLater();
        return;
    }

    if (auto comp = m_entity.TryGetComponent<Blainn::StimulusComponent>())
    {
        comp->tag = ToEASTLString(m_tag->GetValue());
    }
}
} // namespace editor