//
// Created by gorev on 25.10.2025.
//

#pragma once

#include "ContentFilterProxyModel.h"
#include "scene/Entity.h"


#include <QList>
#include <QString>

struct EntityNode
{
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
        name = newName;
        // TODO: update entity name in engine
    }


    const QString &GetName() const
    {
        return name;
    }


    QVector<EntityNode *> children = {}; // can't use smart ptr because qDeleteAll uses raw ptr
    EntityNode *m_parent = nullptr;

    Blainn::Entity &GetEntity()
    {
        return m_entity;
    }

private:
    QString name;
    Blainn::Entity m_entity;
};
