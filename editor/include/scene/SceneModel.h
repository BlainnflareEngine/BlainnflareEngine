//
// Created by gorev on 25.10.2025.
//

#pragma once
#include "EntityNode.h"
#include <QAbstractItemModel>

class SceneModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit SceneModel(QObject *parent = nullptr);
    ~SceneModel() override;

    QModelIndex addNewEntity(const QModelIndex &parentIndex = QModelIndex());

    QModelIndex index(int row, int column, const QModelIndex &parent) const override;

    QModelIndex parent(const QModelIndex &child) const override;

    int rowCount(const QModelIndex &parent) const override;

    int columnCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    bool IsNameDuplicate(const QString& name, const QModelIndex& excludeIndex) const;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    QVector<EntityNode *> m_rootNodes;
};
