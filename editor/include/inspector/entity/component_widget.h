//
// Created by gorev on 16.11.2025.
//

#pragma once
#include "PrefabSubsystem.h"
#include "component_widget_base.h"
#include "scene/Entity.h"
#include "themed_panel.h"
#include "components/PrefabComponent.h"


class QLabel;
class QPushButton;
namespace editor
{

template <class ComponentType> class component_widget : public component_widget_base
{
public:
    explicit component_widget(const Blainn::Entity &entity, const QString &name, QWidget *parent = nullptr)
        : component_widget_base(entity, name, parent)
        , m_componentType(entt::type_hash<ComponentType>::value())
    {
        connect(this, &component_widget::ComponentChanged, this, &component_widget_base::ReportPrefabChanged);
    }

    void ReportPrefabChanged() override
    {
            Blainn::PrefabSubsystem::RegisterComponentOverride(m_entity, m_componentType);
    }

protected:
    entt::id_type m_componentType;
};

} // namespace editor