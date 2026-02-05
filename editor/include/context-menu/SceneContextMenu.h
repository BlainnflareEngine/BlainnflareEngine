#pragma once

#include "scene/Entity.h"

#include <QKeySequence>
#include <QObject>
#include <QPoint>


class QTreeWidgetItem;
class QMenu;
class QAction;

namespace editor
{
class scene_hierarchy_widget;

class SceneContextMenu : public QObject
{
    Q_OBJECT

public:
    explicit SceneContextMenu(scene_hierarchy_widget &treeWidget, QObject *parent = nullptr);

    void RenameEntity();
    void DuplicateEntity();
    void DeleteCurrentEntity();

    QKeySequence GetRenameKey() const
    {
        return QKeySequence(Qt::Key_F2);
    }
    QKeySequence GetDeleteKey() const
    {
        return QKeySequence(Qt::Key_Delete);
    }
    QKeySequence GetDuplicateKey() const
    {
        return QKeySequence(Qt::CTRL + Qt::Key_D);
    }

    void OpenMenu(const QPoint &globalPos, QTreeWidgetItem *item = nullptr);

public slots:
    void OnContextMenu(const QPoint &pos);

private:
    void AddEntity(QTreeWidgetItem *parentItem = nullptr);
    void AddCamera(QTreeWidgetItem *parentItem = nullptr);
    void AddSkybox(QTreeWidgetItem *parentItem = nullptr);
    void AddDirectionalLight(QTreeWidgetItem *parentItem = nullptr);
    void AddPointLight(QTreeWidgetItem *parentItem = nullptr);
    void AddSpotLight(QTreeWidgetItem *parentItem = nullptr);

    void CopyUUIDToClipboard(QTreeWidgetItem *item) const;

    scene_hierarchy_widget &m_treeWidget;

    QAction *m_duplicateAction = nullptr;
    QAction *m_deleteAction = nullptr;
    QAction *m_renameAction = nullptr;

    QTreeWidgetItem *m_currentItem = nullptr;
};
} // namespace editor