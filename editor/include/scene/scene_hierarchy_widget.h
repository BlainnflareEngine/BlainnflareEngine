//
// Created by gorev on 25.10.2025.
//

#pragma once


#include "context-menu/AddToSceneContextMenu.h"


#include <QTreeView>


class SceneItemModel;
namespace editor
{
QT_BEGIN_NAMESPACE
namespace Ui
{
class scene_hierarchy_widget;
}
QT_END_NAMESPACE

class scene_hierarchy_widget : public QTreeView
{
    Q_OBJECT

public:
    explicit scene_hierarchy_widget(QWidget *parent = nullptr);
    ~scene_hierarchy_widget() override;

    void OpenContextMenu(const QPoint &position);

    SceneItemModel &GetSceneModel();

private:
    Ui::scene_hierarchy_widget *ui;
    SceneItemModel *m_sceneModel;
    AddToSceneContextMenu *m_addToSceneMenu;
};
}; // namespace editor
