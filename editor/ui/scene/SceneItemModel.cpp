//
// Created by gorev on 25.10.2025.
//

#include "SceneItemModel.h"

#include "FileSystemUtils.h"

#include <QMessageBox>


editor::SceneItemModel::SceneItemModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}


editor::SceneItemModel::~SceneItemModel()
{
    qDeleteAll(m_rootNodes);
}


QModelIndex editor::SceneItemModel::AddNewEntity(const Blainn::Entity &entity, const QModelIndex &parentIndex)
{
    EntityNode *parentNode = nullptr;
    QVector<EntityNode *> *targetCollection = &m_rootNodes;

    if (parentIndex.isValid())
    {
        parentNode = static_cast<EntityNode *>(parentIndex.internalPointer());
        targetCollection = &parentNode->children;
    }

    EntityNode *newNode = new EntityNode(entity, parentNode);

    newNode->SetName(editor::ToQString(entity.Name()));

    int newRow = targetCollection->size();
    beginInsertRows(parentIndex, newRow, newRow);
    targetCollection->append(newNode);
    endInsertRows();

    return index(newRow, 0, parentIndex);
}


QModelIndex editor::SceneItemModel::index(int row, int column, const QModelIndex &parent) const
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


QModelIndex editor::SceneItemModel::parent(const QModelIndex &child) const
{
    if (!child.isValid()) return QModelIndex();

    EntityNode *childNode = static_cast<EntityNode *>(child.internalPointer());
    EntityNode *parentNode = childNode->m_parent;

    if (!parentNode) return QModelIndex();

    EntityNode *grandParent = parentNode->m_parent;
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


int editor::SceneItemModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) return m_rootNodes.size();


    EntityNode *parentNode = static_cast<EntityNode *>(parent.internalPointer());
    return parentNode ? parentNode->children.size() : 0;
}


int editor::SceneItemModel::columnCount(const QModelIndex &parent) const
{
    // here we should return column count
    // (we can have two rows for instance - first entity name, second entity ID)
    // we have only 1 column for now

    Q_UNUSED(parent);
    return 1;
}


QVariant editor::SceneItemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole) return QVariant();

    EntityNode *node = static_cast<EntityNode *>(index.internalPointer());

    // we display only name for now
    // if we will use more than 1 column we should return more info
    return node->GetName();
}


bool editor::SceneItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
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


bool editor::SceneItemModel::IsNameDuplicate(const QString &name, const QModelIndex &excludeIndex) const
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


Qt::ItemFlags editor::SceneItemModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);

    if (index.isValid())
    {
        return defaultFlags | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }

    return defaultFlags;
}


EntityNode *editor::SceneItemModel::GetNodeFromIndex(const QModelIndex &index)
{
    if (index.isValid()) return static_cast<EntityNode *>(index.internalPointer());

    return nullptr;
}


QModelIndex editor::SceneItemModel::FindIndexByEntity(SceneItemModel *model, const Blainn::Entity &entity)
{
    // TODO: recursive deletion will have bad performance in future
    // This is future Ivan problem :)
    return FindIndexByEntityRecursive(model, QModelIndex(), entity);
}


bool editor::SceneItemModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (row < 0 || row + count > rowCount(parent))
    {
        return false;
    }

    beginRemoveRows(parent, row, row + count - 1);

    if (parent.isValid())
    {
        EntityNode *parentNode = GetNodeFromIndex(parent);
        if (!parentNode)
        {
            endRemoveRows();
            return false;
        }

        for (int i = row + count - 1; i >= row; --i)
        {
            EntityNode *nodeToRemove = parentNode->children.takeAt(i);
            delete nodeToRemove;
        }
    }
    else
    {
        for (int i = row + count - 1; i >= row; --i)
        {
            EntityNode *nodeToRemove = m_rootNodes.takeAt(i);
            delete nodeToRemove;
        }
    }

    endRemoveRows();
    return true;
}


QModelIndex editor::SceneItemModel::FindIndexByEntityRecursive(SceneItemModel *model, const QModelIndex &parent,
                                                               const Blainn::Entity &entity)
{
    for (int row = 0; row < model->rowCount(parent); ++row)
    {
        QModelIndex currentIndex = model->index(row, 0, parent);
        if (!currentIndex.isValid()) continue;

        EntityNode *node = GetNodeFromIndex(currentIndex);

        if (node && node->GetEntity() == entity)
        {
            return currentIndex;
        }

        if (auto foundIndex = FindIndexByEntityRecursive(model, currentIndex, entity); foundIndex.isValid())
        {
            return foundIndex;
        }
    }

    return QModelIndex();
}
