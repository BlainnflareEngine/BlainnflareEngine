//
// Created by gorev on 24.09.2025.
//

#include "ContextMenu/FileContextMenu.h"

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

    QAction *openAction = menu.addAction("Open file");
    QAction *deleteAction = menu.addAction("Delete file");
    QAction *showExplorerAction = menu.addAction("Show in explorer");
    QAction *selectedAction = menu.exec(m_parent.viewport()->mapToGlobal(pos));

    if (selectedAction == openAction) OpenFileExternal(path);
    else if (selectedAction == deleteAction) DeleteFile(path);
    else if (selectedAction == showExplorerAction) OpenFileExplorer(path);
}


void FileContextMenu::DirectoryContext(const QPoint &pos, const QString &path) const
{
    QMenu menu;

    QAction *openAction = menu.QWidget::addAction("Open folder");
    QAction *deleteAction = menu.QWidget::addAction("Delete folder");
    QAction *explorerAction = menu.QWidget::addAction("Show in explorer");
    QAction *selectedAction = menu.exec(m_parent.viewport()->mapToGlobal(pos));
    menu.adjustSize();

    eastl::vector<QAbstractItemView *> itemViews = m_additionalViews;
    itemViews.push_back(&m_parent);

    if (openAction == selectedAction) OpenFolder(path, itemViews);
    else if (selectedAction == deleteAction) DeleteFolder(path);
    else if (selectedAction == explorerAction) OpenFolderExplorer(path);
}


void FileContextMenu::OnContextMenu(const QPoint &pos) const
{
    QModelIndex modelIndex = m_parent.indexAt(pos);
    if (!modelIndex.isValid()) return;

    QFileSystemModel *model = qobject_cast<QFileSystemModel *>(m_parent.model());
    if (!model) return;

    QString filePath = model->filePath(modelIndex);
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