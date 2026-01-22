//
// Created by gorev on 22.01.2026.
//

#include "../../../include/inspector/entity/directional_light_widget.h"

#include "Render/DebugRenderer.h"
#include "scene/EntityTemplates.h"
#include "scene/BasicComponents.h"

namespace editor
{
directional_light_widget::directional_light_widget(const Blainn::Entity &entity, QWidget *parent)
    : component_widget_base(entity, "Directional light", parent)
{
}

void directional_light_widget::DeleteComponent()
{
    // auto component = m_entity.TryGetComponent<>();

    deleteLater();
}

void directional_light_widget::OnUpdate()
{
    using namespace Blainn;

    component_widget_base::OnUpdate();

    if (!RenderSubsystem::GetInstance().DebugEnabled()) return;

    auto transform = m_entity.TryGetComponent<TransformComponent>();
    if (!transform) return;

    Vec3 from = transform->GetTranslation();
    Vec3 to = from + transform->GetForwardVector();

    RenderSubsystem::GetInstance().GetDebugRenderer().DrawArrow(from, to, {1, 1, 0, 1}, 0.2);
}
} // namespace editor