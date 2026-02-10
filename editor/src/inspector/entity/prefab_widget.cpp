//
// Created by gorev on 08.02.2026.
//

#include "inspector/entity/prefab_widget.h"

#include "components/PrefabComponent.h"
#include "scene/EntityTemplates.h"

#include <QHBoxLayout>
#include <QPushButton>

namespace editor
{
prefab_widget::prefab_widget(const Blainn::Entity &entity, QWidget *parent)
    : component_widget(entity, "Prefab", parent)
{
    m_applyChanges = new QPushButton("Apply");
    m_revertChanges = new QPushButton("Revert");

    m_applyChanges->setMinimumHeight(10);
    m_revertChanges->setMinimumHeight(10);

    m_applyChanges->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_revertChanges->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);


    if (auto component = m_entity.TryGetComponent<Blainn::PrefabComponent>())
    {
        bool enableButtons = !component->Overrides.empty();
        m_applyChanges->setEnabled(enableButtons);
        m_revertChanges->setEnabled(enableButtons);
    }
    else
    {
        m_applyChanges->setEnabled(false);
        m_revertChanges->setEnabled(false);
    }

    layout()->addWidget(m_applyChanges);
    layout()->addWidget(m_revertChanges);

    connect(m_applyChanges, &QPushButton::clicked, this,
            [this]()
            {
                Blainn::PrefabSubsystem::ApplyPrefabOverrides(m_entity);
                m_applyChanges->setEnabled(false);
                m_revertChanges->setEnabled(false);
            });

    connect(m_revertChanges, &QPushButton::clicked, this,
            [this]()
            {
                Blainn::PrefabSubsystem::RevertPrefabOverrides(m_entity);
                m_applyChanges->setEnabled(false);
                m_revertChanges->setEnabled(false);
            });
}


void prefab_widget::DeleteComponent()
{
    if (m_entity.IsValid()) m_entity.RemoveComponentIfExists<Blainn::PrefabComponent>();
    deleteLater();
}
} // namespace editor