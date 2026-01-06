//
// Created by gorev on 25.10.2025.
//

#pragma once
#include "../content-browser/folder-content/ContentFilterProxyModel.h"


#include <QKeyEvent>
#include <QObject>


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

class AddToSceneContextMenu : public QObject
{
    Q_OBJECT

public:
    AddToSceneContextMenu(scene_hierarchy_widget &treeView, QObject *parent = nullptr);

    void OpenMenu(const QPoint &pos, const QModelIndex &index = QModelIndex());

    void AddEntity(const QModelIndex &index);
    void AddCamera(const QModelIndex &index);
    void AddSkybox(const QModelIndex &index);
    void RenameEntity(const QModelIndex &index) const;
    void DeleteEntity(const QModelIndex &index);

    QKeySequence &GetRenameKey();
    QKeySequence &GetDeleteKey();

public slots:
    void OnContextMenu(const QPoint &pos);

private:
    scene_hierarchy_widget &m_treeView;

    QKeySequence m_renameKey = QKeySequence(Qt::Key_F2);
    QKeySequence m_deleteKey = QKeySequence(Qt::Key_Delete);
};

} // namespace editor