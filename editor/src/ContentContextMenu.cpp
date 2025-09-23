//
// Created by gorev on 23.09.2025.
//

#include "ContentContextMenu.h"

#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <qabstractitemview.h>

namespace editor
{
ContentContextMenu::ContentContextMenu(QAbstractItemView *parent)
    : QObject(parent)
    , parent(parent)
{
}


void ContentContextMenu::onContextMenu(const QPoint &pos)
{
    QModelIndex index = parent->indexAt(pos);

    QFileSystemModel *model = qobject_cast<QFileSystemModel *>(parent->model());
    if (!model) return;

    QString dirPath;

    if (index.isValid())
    {
        // Клик по элементу — если это папка, берем ее путь,
        // иначе берем путь родителя (текущей папки)
        if (model->isDir(index)) dirPath = model->filePath(index);
        else dirPath = model->filePath(index.parent());
    }
    else
    {
        dirPath = model->rootPath();
    }

    QMenu menu;
    QAction *createFolderAction = menu.addAction("Create folder");

    QAction *selectedAction = menu.exec(parent->viewport()->mapToGlobal(pos));
    if (selectedAction == createFolderAction)
    {
        bool ok = false;
        QString folderName =
            QInputDialog::getText(parent, "Create folder", "Folder name:", QLineEdit::Normal, QString(), &ok);
        if (ok && !folderName.isEmpty())
        {
            QDir dir(dirPath);
            if (!dir.mkdir(folderName))
            {
                QMessageBox::warning(parent, "Ошибка", "Не удалось создать папку");
            }
        }
    }
}
} // namespace editor