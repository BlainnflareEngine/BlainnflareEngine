//
// Created by gorev on 20.11.2025.
//

#pragma once
#include "aliases.h"


#include <QString>


namespace Blainn
{
class Entity;
}
namespace editor
{

class SceneMeta
{
public:
    SceneMeta(const QString &relativeScenePath);
    ~SceneMeta();

    void AddEntity(Blainn::Entity &entity, int position);
    void RemoveEntity(Blainn::uuid ID);
    void SetEntityPosition(Blainn::Entity &entity, int position);


    void Save();
    bool HasEntity(Blainn::Entity &entity) const;
    int GetEntityPosition(Blainn::Entity &entity) const;

private:
    QString m_filePath;
    YAML::Node m_meta;
    eastl::vector<YAML::Node> m_entityNodes;
};


} // namespace editor