//
// Created by gorev on 08.02.2026.
//

#include "inspector/entity/prefab_widget.h"

#include "components/PrefabComponent.h"
#include "scene/EntityTemplates.h"

namespace editor
{
prefab_widget::prefab_widget(const Blainn::Entity &entity, QWidget *parent)
    : component_widget_base(entity, "Prefab", parent)
{
}


void prefab_widget::DeleteComponent()
{
    if (m_entity.IsValid()) m_entity.RemoveComponentIfExists<Blainn::PrefabComponent>();
    deleteLater();
}
} // namespace editor