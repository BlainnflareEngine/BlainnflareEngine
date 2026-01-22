//
// Created by gorev on 25.10.2025.
//

#include "context-menu/SceneContextMenu.h"

#include "Engine.h"
#include "components/CameraComponent.h"
#include "components/SkyboxComponent.h"
#include "scene/EntityTemplates.h"
#include "SceneItemModel.h"
#include "oclero/qlementine/widgets/Menu.hpp"
#include "scene_hierarchy_widget.h"
#include "components/LightComponent.h"

#include <QClipboard>
#include <QKeyEvent>
#include <QPushButton>

namespace editor
{

SceneContextMenu::SceneContextMenu(scene_hierarchy_widget &treeView, QObject *parent)
    : QObject(parent)
    , m_treeView(treeView)
{
    m_duplicateAction = new QAction("Duplicate", this);
    m_duplicateAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));

    m_deleteAction = new QAction("Delete", this);
    m_deleteAction->setShortcut(QKeySequence::Delete);

    m_renameAction = new QAction("Rename", this);
    m_renameAction->setShortcut(QKeySequence(Qt::Key_F2));

    m_treeView.addAction(m_duplicateAction);
    m_treeView.addAction(m_deleteAction);
    m_treeView.addAction(m_renameAction);

    connect(m_duplicateAction, &QAction::triggered, this,
            [this]()
            {
                if (m_treeView.selectionModel()->selectedRows().isEmpty()) return;
                DuplicateEntity(m_treeView.selectionModel()->selectedRows().first());
            });
    connect(m_deleteAction, &QAction::triggered, this,
            [this]()
            {
                if (m_treeView.selectionModel()->selectedRows().isEmpty()) return;
                DeleteEntity(m_treeView.selectionModel()->selectedRows().first());
            });
    connect(m_renameAction, &QAction::triggered, this,
            [this]()
            {
                if (m_treeView.selectionModel()->selectedRows().isEmpty()) return;
                RenameEntity(m_treeView.selectionModel()->selectedRows().first());
            });
}


void SceneContextMenu::OpenMenu(const QPoint &pos, const QModelIndex &index)
{
    QMenu *menu = new QMenu(nullptr);

    QAction *createEntityAction = menu->addAction(index.isValid() ? "Create child entity" : "Create entity");
    QAction *createCameraAction = menu->addAction("Create camera");
    QAction *createSkyboxAction = menu->addAction("Create skybox");
    QAction *createDirectLightAction = menu->addAction("Create directional light");

    if (index.isValid())
    {
        menu->addSeparator();
        menu->addAction(m_renameAction);
        menu->addAction(m_duplicateAction);
        menu->addAction("Copy ID");
        menu->addAction(m_deleteAction);
    }

    if (createEntityAction)
        connect(createEntityAction, &QAction::triggered, this, [this, index]() { AddEntity(index); });

    if (createCameraAction)
        connect(createCameraAction, &QAction::triggered, this, [this, index]() { AddCamera(index); });

    if (createSkyboxAction)
        connect(createSkyboxAction, &QAction::triggered, this, [this, index]() { AddSkybox(index); });

    if (createDirectLightAction)
        connect(createDirectLightAction, &QAction::triggered, this, [this, index]() { AddDirectionalLight(index); });

    if (index.isValid())
    {
        auto clipboardAction = menu->actions().at(menu->actions().size() - 2);
        connect(clipboardAction, &QAction::triggered, this, [this, index]() { CopyUUIDToClipboard(index); });
    }

    connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
    menu->popup(pos);
}


void SceneContextMenu::OnContextMenu(const QPoint &pos)
{
    QModelIndex index = m_treeView.indexAt(pos);

    QPoint menuPos = m_treeView.mapToGlobal(pos);

    if (index.isValid())
    {
        OpenMenu(menuPos, index);
    }
    else
    {
        OpenMenu(menuPos);
    }
}


void SceneContextMenu::AddEntity(const QModelIndex &index)
{
    if (index.isValid())
    {
        Blainn::Entity parent = SceneItemModel::GetNodeFromIndex(index)->GetEntity();

        if (parent.IsValid()) Blainn::Engine::GetActiveScene()->CreateChildEntity(parent, "Entity", false, true);
        else BF_ERROR("Parent entity is invalid.");
    }
    else
    {
        Blainn::Engine::GetActiveScene()->CreateEntity("Entity", false, true);
    }
}


void SceneContextMenu::AddCamera(const QModelIndex &index)
{
    if (index.isValid())
    {
        Blainn::Entity parent = SceneItemModel::GetNodeFromIndex(index)->GetEntity();

        if (parent.IsValid())
        {
            auto entity = Blainn::Engine::GetActiveScene()->CreateChildEntity(parent, "Camera", false, true);
            entity.AddComponent<Blainn::TransformComponent>();
            auto &cam = entity.AddComponent<Blainn::CameraComponent>();
            cam.camera.Reset(75.0f, 16 / 9, 0.1, 1000);
        }
        else BF_ERROR("Parent entity is invalid.");
    }
    else
    {
        auto entity = Blainn::Engine::GetActiveScene()->CreateEntity("Camera", false, true);
        entity.AddComponent<Blainn::TransformComponent>();
        auto &cam = entity.AddComponent<Blainn::CameraComponent>();
        cam.camera.Reset(75.0f, 16 / 9, 0.1, 1000);
    }
}


void SceneContextMenu::AddSkybox(const QModelIndex &index)
{
    if (index.isValid())
    {
        Blainn::Entity parent = SceneItemModel::GetNodeFromIndex(index)->GetEntity();

        if (parent.IsValid())
        {
            auto entity = Blainn::Engine::GetActiveScene()->CreateChildEntity(parent, "Skybox", false, true);
            entity.AddComponent<Blainn::SkyboxComponent>();
        }
        else BF_ERROR("Parent entity is invalid.");
    }
    else
    {
        auto entity = Blainn::Engine::GetActiveScene()->CreateEntity("Skybox", false, true);
        entity.AddComponent<Blainn::SkyboxComponent>();
    }
}


void SceneContextMenu::AddDirectionalLight(const QModelIndex &index)
{
    if (index.isValid())
    {
        Blainn::Entity parent = SceneItemModel::GetNodeFromIndex(index)->GetEntity();

        if (parent.IsValid())
        {
            auto entity = Blainn::Engine::GetActiveScene()->CreateChildEntity(parent, "Directional light", false, true);
            entity.AddComponent<Blainn::TransformComponent>();
            entity.AddComponent<Blainn::DirectionalLightComponent>();
        }
        else BF_ERROR("Parent entity is invalid.");
    }
    else
    {
        auto entity = Blainn::Engine::GetActiveScene()->CreateEntity("Directional light", false, true);
        entity.AddComponent<Blainn::TransformComponent>();
        entity.AddComponent<Blainn::DirectionalLightComponent>();
    }
}


void SceneContextMenu::RenameEntity(const QModelIndex &index) const
{
    m_treeView.edit(index);
}


void SceneContextMenu::DuplicateEntity(const QModelIndex &index) const
{
    auto sceneModel = SceneItemModel::GetNodeFromIndex(index);

    if (!sceneModel) return;

    Blainn::Engine::GetActiveScene()->DuplicateEntity(sceneModel->GetEntity());
}


void SceneContextMenu::DeleteEntity(const QModelIndex &index)
{
    auto sceneModel = SceneItemModel::GetNodeFromIndex(index);

    if (!sceneModel) return;

    Blainn::Engine::GetActiveScene()->SubmitToDestroyEntity(sceneModel->GetEntity());
}


void SceneContextMenu::CopyUUIDToClipboard(const QModelIndex &index)
{
    auto sceneModel = SceneItemModel::GetNodeFromIndex(index);
    if (!sceneModel) return;

    QGuiApplication::clipboard()->setText(sceneModel->GetUUID().str().c_str());
}


QKeySequence &SceneContextMenu::GetRenameKey()
{
    return m_renameKey;
}


QKeySequence &SceneContextMenu::GetDeleteKey()
{
    return m_deleteKey;
}
} // namespace editor