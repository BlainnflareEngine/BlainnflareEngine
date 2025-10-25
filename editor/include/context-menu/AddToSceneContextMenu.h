//
// Created by gorev on 25.10.2025.
//

#pragma once
#include "ContentFilterProxyModel.h"


#include <QObject>


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

public slots:
    void OnContextMenu(const QPoint &pos);

private:
    void AddEntity(const QModelIndex &index);

    scene_hierarchy_widget &m_treeView;
};

} // namespace editor