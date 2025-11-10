//
// Created by gorev on 25.10.2025.
//

#include "context-menu/AddToSceneContextMenu.h"

#include "Engine.h"
#include "SceneItemModel.h"
#include "oclero/qlementine/widgets/Menu.hpp"
#include "scene_hierarchy_widget.h"

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
    QMenu menu;
    menu.move(pos);

    QAction *createEntityAction = menu.addAction("Create entity");

    if (QAction *selectedAction = menu.exec())
    {
        if (selectedAction == createEntityAction) AddEntity(index);
    }
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
    QModelIndex newIndex;

    if (index.isValid())
    {
        // newIndex = m_treeView.GetSceneModel().AddNewEntity(index);
    }
    else
    {
        auto a = Blainn::Engine::GetActiveScene();
        auto entity = Blainn::Engine::GetActiveScene()->CreateEntity("Entity");

        // TODO: mb use delegate for this
        newIndex = m_treeView.GetSceneModel().AddNewEntity(entity);
    }

    if (newIndex.isValid())
    {
        m_treeView.setCurrentIndex(newIndex);

        if (index.isValid())
        {
            m_treeView.expand(index);
        }

        m_treeView.edit(newIndex);
    }
}
} // namespace editor