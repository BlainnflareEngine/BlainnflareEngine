#pragma once

#include "SceneMeta.h"
#include "context-menu/SceneContextMenu.h"
#include "scene/Scene.h"
#include "scene/SceneEvent.h"

#include <QTreeWidget>
#include <QTimer>

namespace editor
{
QT_BEGIN_NAMESPACE
namespace Ui
{
class scene_hierarchy_widget;
}
QT_END_NAMESPACE

class scene_hierarchy_widget : public QTreeWidget
{
    Q_OBJECT

public:
    explicit scene_hierarchy_widget(QWidget *parent = nullptr);
    ~scene_hierarchy_widget() override;

    void OpenContextMenu(const QPoint &position);

    void OnEntityCreated(const Blainn::SceneEventPointer &event);
    void OnEntityDestroyed(const Blainn::SceneEventPointer &event);
    void OnSceneChanged(const Blainn::SceneEventPointer &event);

    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

    Blainn::uuid GetUUIDFromItem(QTreeWidgetItem *item) const;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    QMimeData* mimeData(const QList<QTreeWidgetItem*>& items) const override;
    Qt::DropActions supportedDropActions() const override;
    void startDrag(Qt::DropActions supportedActions) override;

private slots:
    void OnItemSelectionChanged();
    void OnItemChanged(QTreeWidgetItem *item, int column);
    void OnItemDoubleClicked(QTreeWidgetItem *item, int column);

private:
    Ui::scene_hierarchy_widget *ui;
    SceneContextMenu *m_addToSceneMenu;
    eastl::shared_ptr<SceneMeta> m_sceneMeta;

    eastl::vector<eastl::pair<Blainn::Scene::EventHandle, Blainn::SceneEventType>> m_sceneEvents;
    eventpp::CallbackList<void(Blainn::uuid)>::Handle m_selectionHandle;

    QPoint m_dragStartPosition;
    bool m_dragging = false;

    QTreeWidgetItem *FindItemByUuid(const Blainn::uuid &uuid) const;
    void AddItemForEntity(const Blainn::Entity &entity, QTreeWidgetItem *parentItem = nullptr);
    void RemoveItemForEntity(const Blainn::uuid &uuid);
    void UpdateItemName(QTreeWidgetItem *item, const QString &newName);
    bool IsDescendant(QTreeWidgetItem *ancestor, QTreeWidgetItem *item) const;
    void BuildTreeFromScene();
    void ChangeSelection(const Blainn::uuid &id);

    void collectExpandedState(QTreeWidgetItem *item, eastl::vector<Blainn::uuid> &uuids) const;
    eastl::vector<Blainn::uuid> SaveExpandedState(QTreeWidgetItem *root = nullptr) const;
    void RestoreExpandedState(const eastl::vector<Blainn::uuid> &uuids);
};
}; // namespace editor