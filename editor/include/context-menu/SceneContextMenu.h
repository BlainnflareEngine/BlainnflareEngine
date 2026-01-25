//
// Created by gorev on 25.10.2025.
//

#pragma once
#include "folder-content/ContentFilterProxyModel.h"


#include <QKeyEvent>
#include <QObject>


class QMenu;
namespace editor
{
class SceneItemModel;
}
namespace editor
{
class scene_hierarchy_widget;
}
namespace editor
{

class SceneContextMenu : public QObject
{
    Q_OBJECT

public:
    SceneContextMenu(scene_hierarchy_widget &treeView, QObject *parent = nullptr);

    void OpenMenu(const QPoint &pos, const QModelIndex &index = QModelIndex());

    void AddEntity(const QModelIndex &index);

    void AddCamera(const QModelIndex &index);
    void AddSkybox(const QModelIndex &index);
    void AddDirectionalLight(const QModelIndex &index);
    void AddPointLight(const QModelIndex &index);

    void RenameEntity(const QModelIndex &index) const;
    void DuplicateEntity(const QModelIndex &index) const;
    void DeleteEntity(const QModelIndex &index);

    void CopyUUIDToClipboard(const QModelIndex &index);

    QKeySequence &GetRenameKey();
    QKeySequence &GetDeleteKey();

public slots:
    void OnContextMenu(const QPoint &pos);

private:
    scene_hierarchy_widget &m_treeView;

    QKeySequence m_renameKey = QKeySequence(Qt::Key_F2);
    QKeySequence m_deleteKey = QKeySequence(Qt::Key_Delete);
    QKeySequence m_duplicateKey = QKeySequence(Qt::CTRL + Qt::Key_D);

    QMenu *m_menu = nullptr;

    QAction *m_duplicateAction = nullptr;
    QAction *m_deleteAction = nullptr;
    QAction *m_renameAction = nullptr;
};

} // namespace editor