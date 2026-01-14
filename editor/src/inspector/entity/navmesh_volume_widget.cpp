#include "entity/navmesh_volume_widget.h"

#include "Render/DebugRenderer.h"
#include "input-widgets/vector3_input_widget.h"

#include <QLayout>
#include "components/NavMeshVolumeComponent.h"
#include "scene/EntityTemplates.h"
#include <Jolt/Geometry/AABox.h>

namespace editor
{
navmesh_volume_widget::navmesh_volume_widget(const Blainn::Entity &entity, QWidget *parent)
    : component_widget_base(entity, "Navmesh volume", parent)
{
    if (!m_entity.IsValid())
    {
        deleteLater();
        return;
    }

    auto &comp = m_entity.GetComponent<Blainn::NavmeshVolumeComponent>();
    Blainn::Vec3 extents = Blainn::Vec3(comp.LocalBounds.GetExtent().GetX(), comp.LocalBounds.GetExtent().GetY(),
                                        comp.LocalBounds.GetExtent().GetZ());
    BF_DEBUG("Loading extents {} {} {}", extents.x, extents.y, extents.z);
    m_extents = new vector3_input_widget("Extents", extents, this);
    m_extents->SetDecimals(2);
    m_extents->SetMinValue(0);
    layout()->addWidget(m_extents);

    connect(m_extents, &vector3_input_widget::ValueChanged, this, &navmesh_volume_widget::OnExtentsChanged);
}


void navmesh_volume_widget::DeleteComponent()
{
    if (m_entity.IsValid()) m_entity.RemoveComponentIfExists<Blainn::NavmeshVolumeComponent>();
    deleteLater();
}
void navmesh_volume_widget::OnUpdate()
{
    component_widget_base::OnUpdate();

    auto &volume = m_entity.GetComponent<Blainn::NavmeshVolumeComponent>();
    auto &transform = m_entity.GetComponent<Blainn::TransformComponent>();
    Blainn::Vec3 min = {volume.LocalBounds.mMin.GetX(), volume.LocalBounds.mMin.GetY(), volume.LocalBounds.mMin.GetZ()};
    Blainn::Vec3 max = {volume.LocalBounds.mMax.GetX(), volume.LocalBounds.mMax.GetY(), volume.LocalBounds.mMax.GetZ()};
    Blainn::RenderSubsystem::GetInstance().GetDebugRenderer().DrawWireBox(transform.GetTransform(), min, max,
                                                                          {0, 1, 0, 1});
}


void navmesh_volume_widget::OnExtentsChanged()
{
    if (!m_entity.IsValid())
    {
        deleteLater();
        return;
    }

    auto &comp = m_entity.GetComponent<Blainn::NavmeshVolumeComponent>();
    Blainn::Vec3 min = m_extents->GetValue() * -1.0f;
    Blainn::Vec3 max = m_extents->GetValue();
    comp.LocalBounds = JPH::AABox::sFromTwoPoints({min.x, min.y, min.z}, {max.x, max.y, max.z});
    BF_DEBUG("New bounds {} {} {}", comp.LocalBounds.GetExtent().GetX(), comp.LocalBounds.GetExtent().GetY(),
             comp.LocalBounds.GetExtent().GetZ());
}
} // namespace editor
