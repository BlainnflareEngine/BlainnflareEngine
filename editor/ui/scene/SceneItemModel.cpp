//
// Created by gorev on 25.10.2025.
//

#include "SceneItemModel.h"

#include <QMessageBox>


SceneItemModel::SceneItemModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}


SceneItemModel::~SceneItemModel()
{
    qDeleteAll(m_rootNodes);
}


QModelIndex SceneItemModel::addNewEntity(const QModelIndex &parentIndex)
{
    EntityNode *parentNode = nullptr;
    QVector<EntityNode *> *targetCollection = &m_rootNodes;

    if (parentIndex.isValid())
    {
        parentNode = static_cast<EntityNode *>(parentIndex.internalPointer());
        targetCollection = &parentNode->children;
    }

    EntityNode *newNode = new EntityNode();
    if (targetCollection->size() == 0) newNode->SetName(QString("Entity"));
    else newNode->SetName(QString("Entity (%1)").arg(targetCollection->size()));
    newNode->parent = parentNode;

    int newRow = targetCollection->size();
    beginInsertRows(parentIndex, newRow, newRow);
    targetCollection->append(newNode);
    endInsertRows();

    return index(newRow, 0, parentIndex);
}


QModelIndex SceneItemModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent)) return QModelIndex();

    EntityNode *parentNode = nullptr;

    if (parent.isValid())
    {
        parentNode = static_cast<EntityNode *>(parent.internalPointer());
    }

    if (parentNode)
    {
        if (row < parentNode->children.size())
        {
            EntityNode *childNode = parentNode->children[row];
            return createIndex(row, column, childNode);
        }
    }
    else
    {
        if (row < m_rootNodes.size())
        {
            EntityNode *rootNode = m_rootNodes[row];
            return createIndex(row, column, rootNode);
        }
    }

    return QModelIndex();
}


QModelIndex SceneItemModel::parent(const QModelIndex &child) const
{
    if (!child.isValid()) return QModelIndex();

    EntityNode *childNode = static_cast<EntityNode *>(child.internalPointer());
    EntityNode *parentNode = childNode->parent;

    if (!parentNode) return QModelIndex();

    EntityNode *grandParent = parentNode->parent;
    int row = 0;

    if (grandParent)
    {
        row = grandParent->children.indexOf(parentNode);
    }
    else
    {
        row = m_rootNodes.indexOf(parentNode);
    }

    if (row == -1) return QModelIndex();

    return createIndex(row, 0, parentNode);
}


int SceneItemModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) return m_rootNodes.size();


    EntityNode *parentNode = static_cast<EntityNode *>(parent.internalPointer());
    return parentNode ? parentNode->children.size() : 0;
}


int SceneItemModel::columnCount(const QModelIndex &parent) const
{
    // here we should return column count
    // (we can have two rows for instance - first entity name, second entity ID)
    // we have only 1 column for now

    Q_UNUSED(parent);
    return 1;
}


QVariant SceneItemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole) return QVariant();

    EntityNode *node = static_cast<EntityNode *>(index.internalPointer());

    // we display only name for now
    // if we will use more than 1 column we should return more info
    return node->GetName();
}


bool SceneItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::EditRole || role == Qt::DisplayRole)
    {
        QString newName = value.toString().trimmed();
        if (newName.isEmpty())
        {
            return false;
        }

        EntityNode *node = static_cast<EntityNode *>(index.internalPointer());
        node->SetName(newName);
        emit dataChanged(index, index);
        return true;
    }
    return false;
}


bool SceneItemModel::IsNameDuplicate(const QString &name, const QModelIndex &excludeIndex) const
{
    QModelIndex parentIndex = excludeIndex.parent();
    EntityNode *parentNode = parentIndex.isValid() ? static_cast<EntityNode *>(parentIndex.internalPointer()) : nullptr;

    const QVector<EntityNode *> &siblings = parentNode ? parentNode->children : m_rootNodes;

    for (int i = 0; i < siblings.size(); ++i)
    {
        if (i == excludeIndex.row()) continue;

        if (siblings[i]->GetName() == name) return true;
    }

    return false;
}


Qt::ItemFlags SceneItemModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) return Qt::NoItemFlags;

    return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}