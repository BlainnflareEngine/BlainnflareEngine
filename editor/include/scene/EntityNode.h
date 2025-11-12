//
// Created by gorev on 25.10.2025.
//

#pragma once

#include "FileSystemUtils.h"
#include "scene/Entity.h"


#include <QList>

namespace editor
{
struct EntityNode : QObject
{
private:
    Q_OBJECT

public:
    EntityNode(const Blainn::Entity &entity, EntityNode *parent = nullptr)
        : m_parent(parent)
        , m_entity(entity)
    {
        children.reserve(4);
    }


    ~EntityNode()
    {
        qDeleteAll(children);
    }


    void SetName(const QString &newName)
    {
        m_tag = newName;

        emit OnTagChanged(m_tag);

        if (m_entity && m_entity.HasComponent<Blainn::TagComponent>())
            m_entity.GetComponent<Blainn::TagComponent>().Tag = ToEASTLString(newName);
    }


    const QString &GetName() const
    {
        return m_tag;
    }


    QVector<EntityNode *> children = {}; // can't use smart ptr because qDeleteAll uses raw ptr
    EntityNode *m_parent = nullptr;

    Blainn::Entity &GetEntity()
    {
        return m_entity;
    }

signals:
    void OnTagChanged(const QString &newTag);

private:
    QString m_tag;
    Blainn::Entity m_entity;
};
} // namespace editor
