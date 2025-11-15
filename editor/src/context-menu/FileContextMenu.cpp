//
// Created by gorev on 24.09.2025.
//

#include "context-menu/FileContextMenu.h"

#include "FileSystemUtils.h"
#include "folder-content/ContentFilterProxyModel.h"

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
    QMenu *menu = new QMenu(nullptr);

    QAction *openAction = menu->addAction("Open");
    QAction *deleteAction = menu->addAction("Delete");
    menu->addSeparator();
    QAction *showExplorerAction = menu->addAction("Show in explorer");

    if (openAction) connect(openAction, &QAction::triggered, [path]() { OpenFileExternal(path); });


    if (deleteAction) connect(deleteAction, &QAction::triggered, [path]() { BDeleteFile(path); });


    if (showExplorerAction) connect(showExplorerAction, &QAction::triggered, [path]() { OpenFileExplorer(path); });


    connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);

    menu->popup(m_parent.viewport()->mapToGlobal(pos));
}


void FileContextMenu::DirectoryContext(const QPoint &pos, const QString &path) const
{
    QMenu *menu = new QMenu(nullptr);

    QAction *openAction = menu->addAction("Open");
    QAction *deleteAction = menu->addAction("Delete");
    menu->addSeparator();
    QAction *explorerAction = menu->addAction("Show in explorer");

    eastl::vector<QAbstractItemView *> itemViews = m_additionalViews;
    itemViews.push_back(&m_parent);

    if (openAction) connect(openAction, &QAction::triggered, [path, itemViews]() { OpenFolder(path, itemViews); });

    if (deleteAction) connect(deleteAction, &QAction::triggered, [path]() { DeleteFolder(path); });

    if (explorerAction) connect(explorerAction, &QAction::triggered, [path]() { OpenFolderExplorer(path); });


    connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);

    menu->popup(m_parent.viewport()->mapToGlobal(pos));
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