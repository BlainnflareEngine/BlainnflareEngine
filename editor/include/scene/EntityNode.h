//
// Created by gorev on 25.10.2025.
//

#pragma once

#include <QList>
#include <QString>

struct EntityNode
{
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

    QVector<EntityNode *> children;
    EntityNode *parent = nullptr;

private:
    QString name;

    // Should we have a reference?
    // Blainn::Entity entity;
};
