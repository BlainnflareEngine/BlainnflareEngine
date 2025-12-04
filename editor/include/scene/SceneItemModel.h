//
// Created by gorev on 25.10.2025.
//

#pragma once
#include "EntityNode.h"
#include "SceneMeta.h"

#include <QAbstractItemModel>

namespace editor
{
inline static const char *const MIME_ENTITY_UUID = "application/x-blainn-entity-uuid";

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

    static EntityNode *GetNodeFromIndex(const QModelIndex &index);

    static QModelIndex FindIndexByEntity(SceneItemModel *model, const Blainn::uuid &id);

    bool removeRows(int row, int count, const QModelIndex &parent) override;

    void SortAccordingToMeta(eastl::shared_ptr<SceneMeta> &meta);

    QStringList mimeTypes() const override;

    Qt::DropActions supportedDropActions() const override;

private:
    QVector<EntityNode *> m_rootNodes;

    void UpdateNodeHierarchy(const Blainn::uuid &nodeUuid, EntityNode *newParent);
    bool FindAndRemoveNode(EntityNode *parent, const Blainn::uuid &targetUuid, EntityNode **foundNode,
                           QVector<EntityNode *> **collection, int *row);
    bool FullRebuildModel();

    static QModelIndex FindIndexByIDRecursive(SceneItemModel *model, const QModelIndex &parent, const Blainn::uuid &id);


    static void SortNodeChildren(EntityNode *node, const eastl::shared_ptr<SceneMeta> &meta);
};
} // namespace editor
