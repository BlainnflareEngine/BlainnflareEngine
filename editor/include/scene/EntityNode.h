//
// Created by gorev on 25.10.2025.
//

#pragma once

#include "FileSystemUtils.h"
#include "scene/Entity.h"
#include "Engine.h"

#include <QList>

namespace editor
{
struct EntityNode : QObject
{
private:
    Q_OBJECT

public:
    EntityNode(const Blainn::uuid &entityID, EntityNode *parent = nullptr)
        : m_parent(parent)
        , m_entityID(entityID)
    {
        children.reserve(2);
    }


    ~EntityNode()
    {
        qDeleteAll(children);
    }


    void SetName(const QString &newName)
    {
        m_tag = newName;

        emit OnTagChanged(m_tag);

        auto entity = GetEntity();
        if (entity && entity.HasComponent<Blainn::TagComponent>())
            entity.GetComponent<Blainn::TagComponent>().Tag = ToEASTLString(newName);
    }


    const QString &GetName() const
    {
        return m_tag;
    }


    QVector<EntityNode *> children = {}; // can't use smart ptr because qDeleteAll uses raw ptr
    EntityNode *m_parent = nullptr;

    Blainn::Entity GetEntity() const
    {
        auto scene = Blainn::Engine::GetActiveScene();
        if (!scene) return Blainn::Entity{};
        return scene->TryGetEntityWithUUID(m_entityID);
    }

    Blainn::uuid& GetUUID()
    {
        return m_entityID;
    }

signals:
    void OnTagChanged(const QString &newTag);

private:
    QString m_tag;
    Blainn::uuid m_entityID;
};
} // namespace editor
