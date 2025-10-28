//
// Created by gorev on 23.09.2025.
//

#ifndef FOLDERS_WIDGET_H
#define FOLDERS_WIDGET_H

#include "../../context-menu/FileContextMenu.h"
#include "folders_tree_view.h"


#include <QWidget>
#include <qdir.h>


class QTreeView;
class QFileSystemModel;
class QVBoxLayout;

namespace editor
{
class IconProvider;
QT_BEGIN_NAMESPACE
namespace Ui
{
class folders_widget;
}
QT_END_NAMESPACE

class folders_widget : public QWidget
{
    Q_OBJECT

public:
    explicit folders_widget(QWidget *parent = nullptr);
    ~folders_widget() override;

    void SetContentDirectory(const QString &contentDirectory);

    QTreeView *GetTreeView() const;

    void AddAdditionalView(QAbstractItemView *view);
    void RemoveAdditionalView(QAbstractItemView *view);

signals:
    void folderSelected(const QString &path);

public slots:
    void onFolderSelectedIndex(const QModelIndex &newSelection);
    void OnFolderSelectedPath(const QString &newPath) const;

private:
    Ui::folders_widget *m_ui;
    folders_tree_view *m_treeView;
    QFileSystemModel *m_fileSystemModel;
    QVBoxLayout *m_layout;
    FileContextMenu *m_fileContextMenu;
    IconProvider *m_iconProvider;
};
} // namespace editor

#endif // FOLDERS_WIDGET_H
