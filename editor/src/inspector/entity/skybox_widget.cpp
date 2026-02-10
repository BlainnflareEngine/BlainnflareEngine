#include "entity/skybox_widget.h"
#include "components/SkyboxComponent.h"
#include "input-widgets/path_input_field.h"
#include "FileSystemUtils.h"
#include "file-system/TextureType.h"
#include "scene/EntityTemplates.h"
#include <QLayout>

namespace editor
{
skybox_widget::skybox_widget(const Blainn::Entity &entity, QWidget *parent)
: component_widget(entity, "Skybox", parent)
{
    m_texture_input = new path_input_field("Texture", formats::supportedTextureFormats, this);
    layout()->addWidget(m_texture_input);

    UpdatePath();
    connect(m_texture_input, &path_input_field::PathChanged, this, &skybox_widget::OnSetNewPath);
}

void skybox_widget::DeleteComponent()
{
    if (m_entity.IsValid()) m_entity.RemoveComponentIfExists<Blainn::SkyboxComponent>();

    deleteLater();
}

void skybox_widget::UpdatePath()
{
    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::SkyboxComponent>()) destroy();

    const QString path =
        QString::fromStdString(Blainn::AssetManager::GetInstance()
                                   .GetTexturePath(*m_entity.GetComponent<Blainn::SkyboxComponent>().textureHandle)
                                   .string());
    m_texture_input->SetPath(path);
}
void skybox_widget::paintEvent(QPaintEvent *event)
{
    BLAINN_PROFILE_FUNC();

    component_widget::paintEvent(event);
}

void skybox_widget::OnSetNewPath(const QString & oldPath, const QString & newPath)
{
    if (newPath.isEmpty()) return;

    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::SkyboxComponent>()) destroy();

    Blainn::Path path = ToString(newPath);
    auto &skybox = m_entity.GetComponent<Blainn::SkyboxComponent>();

    if (Blainn::AssetManager::GetInstance().HasTexture(path))
        skybox.textureHandle = Blainn::AssetManager::GetInstance().GetTexture(path);
    else
        skybox.textureHandle = Blainn::AssetManager::GetInstance().LoadTexture(path, Blainn::TextureType::CUBEMAP);
}

} // namespace editor


