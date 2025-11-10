//
// Created by gorev on 25.10.2025.
//

#pragma once
#include "EntityNode.h"
#include <QAbstractItemModel>

namespace editor
{
class SceneItemModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit SceneItemModel(QObject *parent = nullptr);
    ~SceneItemModel() override;

    QModelIndex AddNewEntity(const Blainn::Entity &entity, const QModelIndex &parentIndex = QModelIndex());

    QModelIndex index(int row, int column, const QModelIndex &parent) const override;

    QModelIndex parent(const QModelIndex &child) const override;

    int rowCount(const QModelIndex &parent) const override;

    int columnCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    bool IsNameDuplicate(const QString &name, const QModelIndex &excludeIndex) const;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    static EntityNode * GetNodeFromIndex(const QModelIndex& index);

private:
    QVector<EntityNode *> m_rootNodes;
};
} // namespace editor
