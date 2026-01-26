//
// Created by gorev on 25.10.2025.
//

#include "SceneItemModel.h"

#include "EASTL/sort.h"
#include "Engine.h"
#include "EntityNodeComparator.h"
#include "FileSystemUtils.h"

#include <QMessageBox>
#include <QMimeData>

constexpr char MIME_ENTITY_UUID[] = "application/x-blainn-entity-uuid";

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
    if (!entity.IsValid()) return QModelIndex();

    EntityNode *parentNode = nullptr;
    QVector<EntityNode *> *targetCollection = &m_rootNodes;

    if (parentIndex.isValid())
    {
        parentNode = static_cast<EntityNode *>(parentIndex.internalPointer());
        targetCollection = &parentNode->children;
    }

    EntityNode *newNode = new EntityNode(entity.GetUUID(), parentNode);

    newNode->SetName(ToQString(entity.Name()));

    int newRow = targetCollection->size();
    beginInsertRows(parentIndex, newRow, newRow);
    targetCollection->append(newNode);
    endInsertRows();

    return index(newRow, 0, parentIndex);
}


QModelIndex editor::SceneItemModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
    {
        return QModelIndex();
    }

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
            return createIndex(row, column, m_rootNodes[row]);
        }
    }

    return QModelIndex();
}


QModelIndex editor::SceneItemModel::parent(const QModelIndex &child) const
{
    if (!child.isValid()) return QModelIndex();

    EntityNode *childNode = static_cast<EntityNode *>(child.internalPointer());
    EntityNode *parentNode = childNode->m_parent;

    if (!parentNode)
    {
        return QModelIndex();
    }

    EntityNode *grandParent = parentNode->m_parent;
    int row = -1;

    if (grandParent)
    {
        row = grandParent->children.indexOf(parentNode);
    }
    else
    {
        row = m_rootNodes.indexOf(parentNode);
    }

    if (row == -1)
    {
        return QModelIndex();
    }

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

    Blainn::Entity entity = node->GetEntity();
    if (entity.IsValid())
    {
        return ToQString(entity.Name());
    }

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
    if (!index.isValid()) return Qt::ItemIsDropEnabled;
    return QAbstractItemModel::flags(index) | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable
           | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}


editor::EntityNode *editor::SceneItemModel::GetNodeFromIndex(const QModelIndex &index)
{
    if (index.isValid()) return static_cast<EntityNode *>(index.internalPointer());

    return nullptr;
}


QModelIndex editor::SceneItemModel::FindIndexByEntity(SceneItemModel *model, const Blainn::uuid &id)
{
    BLAINN_PROFILE_FUNC();
    // TODO: recursive deletion will have bad performance in future
    // This is future Ivan problem :)
    return FindIndexByIDRecursive(model, QModelIndex(), id);
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


void editor::SceneItemModel::SortAccordingToMeta(eastl::shared_ptr<SceneMeta> &meta)
{
    if (!meta)
    {
        BF_WARN("No meta data found.");
        return;
    }

    beginResetModel();

    EntityNodeComparator comparator(meta);
    eastl::sort(m_rootNodes.begin(), m_rootNodes.end(), comparator);

    for (EntityNode *rootNode : m_rootNodes)
    {
        SortNodeChildren(rootNode, meta);
    }

    endResetModel();

    BF_INFO("Scene model sorted according to meta data");
}


bool editor::SceneItemModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                                          const QModelIndex &parent)
{
    if (action != Qt::MoveAction || !data || !data->hasFormat(MIME_ENTITY_UUID)) return false;

    QByteArray encoded = data->data(MIME_ENTITY_UUID);
    if (encoded.size() != sizeof(Blainn::uuid)) return false;

    Blainn::uuid uuid;
    memcpy(&uuid, encoded.constData(), sizeof(uuid));

    auto scene = Blainn::Engine::GetActiveScene();
    if (scene)
    {
        Blainn::Entity entity = scene->TryGetEntityWithUUID(uuid);
        if (entity.IsValid())
        {
            Blainn::Entity newParent;
            if (parent.isValid())
            {
                EntityNode *parentNode = static_cast<EntityNode *>(parent.internalPointer());
                newParent = scene->TryGetEntityWithUUID(parentNode->GetUUID());
            }
            if (newParent) scene->ParentEntity(entity, newParent);
            else scene->UnparentEntity(entity);
        }
    }

    beginResetModel();
    qDeleteAll(m_rootNodes);
    m_rootNodes.clear();
    endResetModel();

    FullRebuildModel();

    return true;
}


QMimeData *editor::SceneItemModel::mimeData(const QModelIndexList &indexes) const
{
    if (indexes.isEmpty()) return nullptr;

    QMimeData *mimeData = new QMimeData();
    QModelIndex index = indexes.first();

    EntityNode *node = static_cast<EntityNode *>(index.internalPointer());
    if (!node) return nullptr;

    QByteArray data;
    data.resize(sizeof(Blainn::uuid));
    memcpy(data.data(), &node->GetUUID(), sizeof(Blainn::uuid));
    mimeData->setData(MIME_ENTITY_UUID, data);

    return mimeData;
}


QStringList editor::SceneItemModel::mimeTypes() const
{
    return {MIME_ENTITY_UUID};
}


Qt::DropActions editor::SceneItemModel::supportedDropActions() const
{
    return Qt::MoveAction;
}


void editor::SceneItemModel::UpdateNodeHierarchy(const Blainn::uuid &nodeUuid, EntityNode *newParent)
{
    EntityNode *node = nullptr;
    QVector<EntityNode *> *oldCollection = &m_rootNodes;
    int oldRow = -1;

    for (int i = 0; i < m_rootNodes.size(); ++i)
    {
        if (m_rootNodes[i]->GetUUID() == nodeUuid)
        {
            node = m_rootNodes[i];
            oldRow = i;
            break;
        }
    }

    if (!node)
    {
        for (EntityNode *root : m_rootNodes)
        {
            if (FindAndRemoveNode(root, nodeUuid, &node, &oldCollection, &oldRow)) break;
        }
    }

    if (!node) return;

    node->m_parent = newParent;

    if (oldRow != -1 && oldCollection)
    {
        oldCollection->remove(oldRow);
    }

    if (newParent)
    {
        newParent->children.append(node);
    }
    else
    {
        m_rootNodes.append(node);
    }
}


bool editor::SceneItemModel::FindAndRemoveNode(EntityNode *parent, const Blainn::uuid &targetUuid,
                                               EntityNode **foundNode, QVector<EntityNode *> **collection, int *row)
{
    for (int i = 0; i < parent->children.size(); ++i)
    {
        EntityNode *child = parent->children[i];
        if (child->GetUUID() == targetUuid)
        {
            *foundNode = child;
            *collection = &parent->children;
            *row = i;
            return true;
        }

        if (FindAndRemoveNode(child, targetUuid, foundNode, collection, row)) return true;
    }
    return false;
}


bool editor::SceneItemModel::FullRebuildModel()
{
    beginResetModel();
    qDeleteAll(m_rootNodes);
    m_rootNodes.clear();

    auto scene = Blainn::Engine::GetActiveScene();
    if (!scene)
    {
        endResetModel();
        return false;
    }

    auto view = scene->GetAllEntitiesWith<Blainn::IDComponent, Blainn::RelationshipComponent>();
    eastl::vector<Blainn::Entity> rootEntities;
    eastl::unordered_map<Blainn::uuid, EntityNode *> nodeMap;

    for (auto [entityHandle, idComp, relationshipComp] : view.each())
    {
        Blainn::Entity entity(entityHandle, scene.get());
        EntityNode *node = new EntityNode(entity.GetUUID(), nullptr);
        nodeMap[entity.GetUUID()] = node;
    }

    for (auto &pair : nodeMap)
    {
        Blainn::Entity entity = scene->TryGetEntityWithUUID(pair.first);
        if (!entity.IsValid()) continue;

        pair.second->SetName(ToQString(entity.Name()));

        Blainn::Entity parent = entity.GetParent();
        if (parent.IsValid())
        {
            auto parentIt = nodeMap.find(parent.GetUUID());
            if (parentIt != nodeMap.end())
            {
                pair.second->m_parent = parentIt->second;
                parentIt->second->children.append(pair.second);
            }
        }
        else
        {
            rootEntities.push_back(entity);
        }
    }

    for (auto &entity : rootEntities)
    {
        auto it = nodeMap.find(entity.GetUUID());
        if (it != nodeMap.end())
        {
            m_rootNodes.append(it->second);
        }
    }

    endResetModel();
    return true;
}


QModelIndex editor::SceneItemModel::FindIndexByIDRecursive(SceneItemModel *model, const QModelIndex &parent,
                                                           const Blainn::uuid &id)
{
    BLAINN_PROFILE_FUNC();
    for (int row = 0; row < model->rowCount(parent); ++row)
    {
        QModelIndex currentIndex = model->index(row, 0, parent);
        if (!currentIndex.isValid()) continue;

        EntityNode *node = GetNodeFromIndex(currentIndex);

        std::string uid = node->GetUUID().bytes();
        std::string uid2 = id.bytes();
        if (node && node->GetUUID() == id)
        {
            return currentIndex;
        }

        if (auto foundIndex = FindIndexByIDRecursive(model, currentIndex, id); foundIndex.isValid())
        {
            return foundIndex;
        }
    }

    return QModelIndex();
}


void editor::SceneItemModel::SortNodeChildren(EntityNode *node, const eastl::shared_ptr<SceneMeta> &meta)
{
    if (!node || node->children.empty()) return;

    EntityNodeComparator comparator(meta);
    eastl::sort(node->children.begin(), node->children.end(), comparator);

    for (EntityNode *child : node->children)
    {
        SortNodeChildren(child, meta);
    }
}