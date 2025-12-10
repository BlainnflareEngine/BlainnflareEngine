//
// Created by gorev on 25.10.2025.
//

#include "context-menu/AddToSceneContextMenu.h"

#include "Engine.h"
#include "SceneItemModel.h"
#include "oclero/qlementine/widgets/Menu.hpp"
#include "scene_hierarchy_widget.h"

#include <QKeyEvent>
#include <QPushButton>

namespace editor
{

AddToSceneContextMenu::AddToSceneContextMenu(scene_hierarchy_widget &treeView, QObject *parent)
    : QObject(parent)
    , m_treeView(treeView)
{
}


void AddToSceneContextMenu::OpenMenu(const QPoint &pos, const QModelIndex &index)
{
    QMenu *menu = new QMenu(nullptr);

    QAction *createEntityAction = menu->addAction("Create entity");
    QAction *editAction = nullptr;
    QAction *deleteAction = nullptr;


    if (index.isValid())
    {
        editAction = menu->addAction("Edit");
        deleteAction = menu->addAction("Delete");

        editAction->setShortcut(m_renameKey);
        deleteAction->setShortcut(m_deleteKey);
    }


    if (createEntityAction)
        connect(createEntityAction, &QAction::triggered, this, [this, index]() { AddEntity(index); });


    if (editAction) connect(editAction, &QAction::triggered, this, [this, index]() { RenameEntity(index); });


    if (deleteAction) connect(deleteAction, &QAction::triggered, this, [this, index]() { DeleteEntity(index); });


    connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);

    menu->popup(pos);
}


void AddToSceneContextMenu::OnContextMenu(const QPoint &pos)
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


void AddToSceneContextMenu::AddEntity(const QModelIndex &index)
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


void AddToSceneContextMenu::RenameEntity(const QModelIndex &index) const
{
    m_treeView.edit(index);
}


void AddToSceneContextMenu::DeleteEntity(const QModelIndex &index)
{
    auto sceneModel = SceneItemModel::GetNodeFromIndex(index);

    if (!sceneModel) return;

    Blainn::Engine::GetActiveScene()->SubmitToDestroyEntity(sceneModel->GetEntity());
}


QKeySequence &AddToSceneContextMenu::GetRenameKey()
{
    return m_renameKey;
}


QKeySequence &AddToSceneContextMenu::GetDeleteKey()
{
    return m_deleteKey;
}
} // namespace editor