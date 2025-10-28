//
// Created by gorev on 25.10.2025.
//

#pragma once
#include <QStyledItemDelegate>

namespace editor
{

class EntityDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit EntityDelegate(QObject *parent = nullptr);

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;


};

} // namespace editor