//
// Created by gorev on 24.09.2025.
//

#include "context-menu/FileContextMenu.h"

#include "../../include/content-browser/folder-content/ContentFilterProxyModel.h"
#include "EASTL/unique_ptr.h"
#include "FileSystemUtils.h"

#include <QFileSystemModel>
#include <QMenu>
#include <qabstractitemview.h>
#include <qfileinfo.h>

namespace editor
{

FileContextMenu::FileContextMenu(QAbstractItemView &parent, const eastl::vector<QAbstractItemView *> &additionalViews)
    : m_parent(parent)
    , m_additionalViews(additionalViews)
{
}


void FileContextMenu::FileContext(const QPoint &pos, const QString &path) const
{
    QMenu menu;

    QAction *openAction = menu.addAction("Open");
    QAction *deleteAction = menu.addAction("Delete");
    menu.addSeparator();
    QAction *showExplorerAction = menu.addAction("Show in explorer");

    if (QAction *selectedAction = menu.exec(m_parent.viewport()->mapToGlobal(pos)))
    {
        if (selectedAction == openAction) OpenFileExternal(path);
        else if (selectedAction == deleteAction) BDeleteFile(path);
        else if (selectedAction == showExplorerAction) OpenFileExplorer(path);
    }
}


void FileContextMenu::DirectoryContext(const QPoint &pos, const QString &path) const
{
    QMenu menu;

    QAction *openAction = menu.QWidget::addAction("Open");
    QAction *deleteAction = menu.QWidget::addAction("Delete");
    menu.addSeparator();
    QAction *explorerAction = menu.QWidget::addAction("Show in explorer");
    menu.adjustSize();

    eastl::vector<QAbstractItemView *> itemViews = m_additionalViews;
    itemViews.push_back(&m_parent);

    if (QAction *selectedAction = menu.exec(m_parent.viewport()->mapToGlobal(pos)))
    {
        if (openAction == selectedAction) OpenFolder(path, itemViews);
        else if (selectedAction == deleteAction) DeleteFolder(path);
        else if (selectedAction == explorerAction) OpenFolderExplorer(path);
    }
}


void FileContextMenu::OnContextMenu(const QPoint &pos) const
{
    QModelIndex index = m_parent.indexAt(pos);
    if (!index.isValid()) return;

    QFileSystemModel *fileSystemModel = nullptr;
    QModelIndex sourceIndex = index;

    if (QAbstractProxyModel *proxyModel = qobject_cast<QAbstractProxyModel *>(m_parent.model()))
    {
        fileSystemModel = qobject_cast<QFileSystemModel *>(proxyModel->sourceModel());
        sourceIndex = proxyModel->mapToSource(index);
    }
    else
    {
        fileSystemModel = qobject_cast<QFileSystemModel *>(m_parent.model());
    }

    if (!fileSystemModel) return;

    QString filePath = fileSystemModel->filePath(sourceIndex);
    QFileInfo fileInfo = QFileInfo(filePath);

    if (fileInfo.isFile())
    {
        FileContext(pos, filePath);
    }
    else if (fileInfo.isDir())
    {
        DirectoryContext(pos, filePath);
    }
}


void FileContextMenu::AddAdditionalView(QAbstractItemView *view)
{
    m_additionalViews.push_back(view);
}


void FileContextMenu::RemoveAdditionalView(QAbstractItemView *view)
{
    m_additionalViews.erase_first(view);
}

} // namespace editor