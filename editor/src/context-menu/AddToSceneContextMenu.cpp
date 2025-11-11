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

        editAction->setShortcut(QKeySequence(Qt::Key_F2));
        deleteAction->setShortcut(QKeySequence(Qt::Key_Delete));
    }

    connect(menu, &QMenu::triggered, this,
            [this, menu, index, createEntityAction, editAction, deleteAction](QAction *selectedAction)
            {
                if (selectedAction == createEntityAction)
                {
                    AddEntity(index);
                }
                else if (editAction && selectedAction == editAction)
                {
                    m_treeView.edit(index);
                }
                else if (deleteAction && selectedAction == deleteAction)
                {
                    BF_DEBUG("Deleting entity");
                }

                menu->deleteLater();
            });

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
    QModelIndex newIndex;

    if (index.isValid())
    {
        // newIndex = m_treeView.GetSceneModel().AddNewEntity(index);
    }
    else
    {
        auto a = Blainn::Engine::GetActiveScene();
        auto entity = Blainn::Engine::GetActiveScene()->CreateEntity("Entity");
    }
    /*// TODO: mb use delegate for this
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
}*/
}
} // namespace editor