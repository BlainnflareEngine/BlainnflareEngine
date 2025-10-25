//
// Created by gorev on 22.10.2025.
//

#include "ContentFilterProxyModel.h"

#include <QFileSystemModel>
#include <qfileinfo.h>


namespace editor
{
ContentFilterProxyModel::ContentFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}


bool ContentFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QFileSystemModel *fileModel = qobject_cast<QFileSystemModel *>(sourceModel());
    if (!fileModel) return true;

    QModelIndex index = fileModel->index(sourceRow, 0, sourceParent);

    QFileInfo fileInfo = fileModel->fileInfo(index);

    if (fileInfo.isFile() && fileInfo.suffix().toLower() == "blainn")
    {
        return false;
    }

    return true;
}
} // namespace editor