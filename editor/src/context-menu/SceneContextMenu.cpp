#include "context-menu/SceneContextMenu.h"
#include "scene_hierarchy_widget.h"
#include "Engine.h"
#include "components/CameraComponent.h"
#include "components/SkyboxComponent.h"
#include "components/LightComponent.h"
#include "scene/EntityTemplates.h"

#include <QMenu>
#include <QClipboard>
#include <QGuiApplication>
#include <QAction>

namespace editor
{

SceneContextMenu::SceneContextMenu(scene_hierarchy_widget &treeWidget, QObject *parent)
    : QObject(parent)
    , m_treeWidget(treeWidget)
{
    m_duplicateAction = new QAction("Duplicate", this);
    m_duplicateAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));
    m_duplicateAction->setShortcutContext(Qt::WidgetShortcut);

    m_deleteAction = new QAction("Delete", this);
    m_deleteAction->setShortcut(QKeySequence::Delete);
    m_deleteAction->setShortcutContext(Qt::WidgetShortcut);

    m_renameAction = new QAction("Rename", this);
    m_renameAction->setShortcut(QKeySequence(Qt::Key_F2));
    m_renameAction->setShortcutContext(Qt::WidgetShortcut);

    m_treeWidget.addAction(m_duplicateAction);
    m_treeWidget.addAction(m_deleteAction);
    m_treeWidget.addAction(m_renameAction);

    connect(m_duplicateAction, &QAction::triggered, this, &SceneContextMenu::DuplicateEntity);
    connect(m_deleteAction, &QAction::triggered, this, &SceneContextMenu::DeleteCurrentEntity);
    connect(m_renameAction, &QAction::triggered, this, &SceneContextMenu::RenameEntity);
}

void SceneContextMenu::OnContextMenu(const QPoint &pos)
{
    QTreeWidgetItem *item = m_treeWidget.itemAt(pos);
    QPoint globalPos = m_treeWidget.mapToGlobal(pos);

    OpenMenu(globalPos, item);
}

void SceneContextMenu::OpenMenu(const QPoint &globalPos, QTreeWidgetItem *item)
{
    m_currentItem = item;

    auto *menu = new QMenu(nullptr);

    QAction *createEntityAction = menu->addAction(item ? "Create child entity" : "Create entity");
    QAction *createCameraAction = menu->addAction("Create camera");
    QAction *createSkyboxAction = menu->addAction("Create skybox");

    menu->addSeparator();
    QAction *createDirectLightAction = menu->addAction("Create directional light");
    QAction *createPointLightAction = menu->addAction("Create point light");
    QAction *createSpotLightAction = menu->addAction("Create spot light");

    if (item)
    {
        menu->addSeparator();
        menu->addAction(m_renameAction);
        menu->addAction(m_duplicateAction);
        menu->addAction("Copy ID");
        menu->addAction(m_deleteAction);
    }

    connect(createEntityAction, &QAction::triggered, this, [this, item]() { AddEntity(item); });

    connect(createCameraAction, &QAction::triggered, this, [this, item]() { AddCamera(item); });

    connect(createSkyboxAction, &QAction::triggered, this, [this, item]() { AddSkybox(item); });

    connect(createDirectLightAction, &QAction::triggered, this, [this, item]() { AddDirectionalLight(item); });

    connect(createPointLightAction, &QAction::triggered, this, [this, item]() { AddPointLight(item); });

    connect(createSpotLightAction, &QAction::triggered, this, [this, item]() { AddSpotLight(item); });

    if (item)
    {
        auto actions = menu->actions();
        QAction *copyIdAction = nullptr;
        for (qsizetype i = actions.size() - 1; i >= 0; --i)
        {
            if (actions[i]->text() == "Copy ID")
            {
                copyIdAction = actions[i];
                break;
            }
        }

        if (copyIdAction)
        {
            connect(copyIdAction, &QAction::triggered, this, [this, item]() { CopyUUIDToClipboard(item); });
        }
    }

    connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
    menu->popup(globalPos);
}


void SceneContextMenu::AddEntity(QTreeWidgetItem *parentItem)
{
    if (parentItem)
    {
        auto uuid = m_treeWidget.GetUUIDFromItem(parentItem);
        auto parent = Blainn::Engine::GetSceneManager().TryGetEntityWithUUID(uuid);

        if (parent.IsValid())
        {
            Blainn::Engine::GetSceneManager().CreateChildEntity(parent, "Entity", false, true);
        }
        else
        {
            BF_ERROR("Parent entity is invalid.");
        }
    }
    else
    {
        Blainn::Engine::GetSceneManager().CreateEntity("Entity", false, true);
    }
}

void SceneContextMenu::AddCamera(QTreeWidgetItem *parentItem)
{
    Blainn::Entity entity;

    if (parentItem)
    {
        auto uuid = m_treeWidget.GetUUIDFromItem(parentItem);
        auto parent = Blainn::Engine::GetSceneManager().TryGetEntityWithUUID(uuid);

        if (parent.IsValid())
        {
            entity = Blainn::Engine::GetSceneManager().CreateChildEntity(parent, "Camera", false, true);
        }
        else
        {
            BF_ERROR("Parent entity is invalid.");
            return;
        }
    }
    else
    {
        entity = Blainn::Engine::GetSceneManager().CreateEntity("Camera", false, true);
    }

    if (entity.IsValid())
    {
        entity.AddComponent<Blainn::TransformComponent>();
        auto &cam = entity.AddComponent<Blainn::CameraComponent>();
        cam.camera.Reset({75.0f, 16.0f / 9.0f, 0.1f, 1000.0f});
    }
}

void SceneContextMenu::AddSkybox(QTreeWidgetItem *parentItem)
{
    Blainn::Entity entity;

    if (parentItem)
    {
        auto uuid = m_treeWidget.GetUUIDFromItem(parentItem);
        auto parent = Blainn::Engine::GetSceneManager().TryGetEntityWithUUID(uuid);

        if (parent.IsValid())
        {
            entity = Blainn::Engine::GetSceneManager().CreateChildEntity(parent, "Skybox", false, true);
        }
        else
        {
            BF_ERROR("Parent entity is invalid.");
            return;
        }
    }
    else
    {
        entity = Blainn::Engine::GetSceneManager().CreateEntity("Skybox", false, true);
    }

    if (entity.IsValid())
    {
        entity.AddComponent<Blainn::SkyboxComponent>();
    }
}

void SceneContextMenu::AddDirectionalLight(QTreeWidgetItem *parentItem)
{
    Blainn::Entity entity;

    if (parentItem)
    {
        auto uuid = m_treeWidget.GetUUIDFromItem(parentItem);
        auto parent = Blainn::Engine::GetSceneManager().TryGetEntityWithUUID(uuid);

        if (parent.IsValid())
        {
            entity = Blainn::Engine::GetSceneManager().CreateChildEntity(parent, "Directional light", false, true);
        }
        else
        {
            BF_ERROR("Parent entity is invalid.");
            return;
        }
    }
    else
    {
        entity = Blainn::Engine::GetSceneManager().CreateEntity("Directional light", false, true);
    }

    if (entity.IsValid())
    {
        entity.AddComponent<Blainn::TransformComponent>();
        entity.AddComponent<Blainn::DirectionalLightComponent>();
    }
}

void SceneContextMenu::AddPointLight(QTreeWidgetItem *parentItem)
{
    Blainn::Entity entity;

    if (parentItem)
    {
        auto uuid = m_treeWidget.GetUUIDFromItem(parentItem);
        auto parent = Blainn::Engine::GetSceneManager().TryGetEntityWithUUID(uuid);

        if (parent.IsValid())
        {
            entity = Blainn::Engine::GetSceneManager().CreateChildEntity(parent, "Point light", false, true);
        }
        else
        {
            BF_ERROR("Parent entity is invalid.");
            return;
        }
    }
    else
    {
        entity = Blainn::Engine::GetSceneManager().CreateEntity("Point light", false, true);
    }

    if (entity.IsValid())
    {
        entity.AddComponent<Blainn::TransformComponent>();
        entity.AddComponent<Blainn::PointLightComponent>();
    }
}

void SceneContextMenu::AddSpotLight(QTreeWidgetItem *parentItem)
{
    Blainn::Entity entity;

    if (parentItem)
    {
        auto uuid = m_treeWidget.GetUUIDFromItem(parentItem);
        auto parent = Blainn::Engine::GetSceneManager().TryGetEntityWithUUID(uuid);

        if (parent.IsValid())
        {
            entity = Blainn::Engine::GetSceneManager().CreateChildEntity(parent, "Spot light", false, true);
        }
        else
        {
            BF_ERROR("Parent entity is invalid.");
            return;
        }
    }
    else
    {
        entity = Blainn::Engine::GetSceneManager().CreateEntity("Spot light", false, true);
    }

    if (entity.IsValid())
    {
        entity.AddComponent<Blainn::TransformComponent>();
        entity.AddComponent<Blainn::SpotLightComponent>();
    }
}


void SceneContextMenu::RenameEntity()
{
    if (auto *item = m_treeWidget.currentItem())
    {
        m_treeWidget.editItem(item);
    }
}

void SceneContextMenu::DuplicateEntity()
{
    if (auto *item = m_treeWidget.currentItem())
    {
        auto uuid = m_treeWidget.GetUUIDFromItem(item);
        auto entity = Blainn::Engine::GetSceneManager().TryGetEntityWithUUID(uuid);

        if (entity.IsValid())
        {
            Blainn::Engine::GetSceneManager().DuplicateEntity(entity);
        }
    }
}

void SceneContextMenu::DeleteCurrentEntity()
{
    if (auto *item = m_treeWidget.currentItem())
    {
        auto uuid = m_treeWidget.GetUUIDFromItem(item);
        auto entity = Blainn::Engine::GetSceneManager().TryGetEntityWithUUID(uuid);

        if (entity.IsValid())
        {
            Blainn::Engine::GetSceneManager().SubmitToDestroyEntity(entity);
        }
    }
}

void SceneContextMenu::CopyUUIDToClipboard(QTreeWidgetItem *item) const
{
    if (!item) return;

    auto uuid = m_treeWidget.GetUUIDFromItem(item);
    QGuiApplication::clipboard()->setText(uuid.str().c_str());
}

} // namespace editor
