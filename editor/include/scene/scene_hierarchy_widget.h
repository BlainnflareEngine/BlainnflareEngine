//
// Created by gorev on 25.10.2025.
//

#pragma once


#include "SceneMeta.h"
#include "context-menu/AddToSceneContextMenu.h"
#include "scene/SceneEvent.h"


#include <QTreeView>


namespace editor
{
class SceneItemModel;

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

    SceneItemModel &GetSceneModel() const;

    void OnEntityCreated(const Blainn::SceneEventPointer &event);
    void OnEntityDestroyed(const Blainn::SceneEventPointer &event);
    void OnSceneChanged(const Blainn::SceneEventPointer &event);

public slots:

    void OnItemDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);

    void OnSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

    void SaveCurrentMeta();
protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    Ui::scene_hierarchy_widget *ui;
    SceneItemModel *m_sceneModel;
    AddToSceneContextMenu *m_addToSceneMenu;
    eastl::shared_ptr<SceneMeta> m_sceneMeta;
};
}; // namespace editor
