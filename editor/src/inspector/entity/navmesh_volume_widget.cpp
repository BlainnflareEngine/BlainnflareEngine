#include "entity/navmesh_volume_widget.h"

#include "input-widgets/vector3_input_widget.h"

#include <QLayout>
#include "components/NavMeshVolumeComponent.h"

namespace editor
{
navmesh_volume_widget::navmesh_volume_widget(const Blainn::Entity &entity, QWidget *parent)
    : component_widget_base(entity, "Navmesh volume", parent)
{
    m_extents = new vector3_input_widget("Extents", Blainn::Vec3::Zero, this);
    m_extents->SetDecimals(3);
    layout()->addWidget(m_extents);
}


void navmesh_volume_widget::DeleteComponent()
{
    if (m_entity.IsValid()) m_entity.RemoveComponentIfExists<Blainn::NavmeshVolumeComponent>();
    deleteLater();
}
} // namespace editor
