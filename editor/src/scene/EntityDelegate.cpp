//
// Created by gorev on 25.10.2025.
//

#include "EntityDelegate.h"

#include "FileSystemUtils.h"
#include "oclero/qlementine/widgets/LineEdit.hpp"

#include <QMessageBox>

namespace editor
{
EntityDelegate::EntityDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}


void EntityDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
    if (!lineEdit) return;

    model->setData(index, lineEdit->text(), Qt::EditRole);
}


QWidget *EntityDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                      const QModelIndex &index) const
{
    QLineEdit *editor = new QLineEdit(parent);
    QString currentText = index.data(Qt::DisplayRole).toString();

    editor->setText(currentText);
    editor->selectAll();

    return editor;
}
} // namespace editor