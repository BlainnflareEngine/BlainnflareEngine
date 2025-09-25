//
// Created by gorev on 23.09.2025.
//

#ifndef FOLDERS_WIDGET_H
#define FOLDERS_WIDGET_H

#include "ContextMenu/FileContextMenu.h"
#include "folders_tree_view.h"


#include <EASTL/unique_ptr.h>
#include <QWidget>
#include <qdir.h>


class QTreeView;
class QFileSystemModel;
class QVBoxLayout;

namespace editor
{
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
    void onFolderSelectedPath(const QString &newPath) const;

private:
    eastl::unique_ptr<Ui::folders_widget> m_ui;
    eastl::unique_ptr<folders_tree_view> m_treeView;
    eastl::unique_ptr<QFileSystemModel> m_fileSystemModel;
    eastl::unique_ptr<QVBoxLayout> m_layout;
    eastl::unique_ptr<FileContextMenu> m_fileContextMenu;
};
} // namespace editor

#endif // FOLDERS_WIDGET_H
