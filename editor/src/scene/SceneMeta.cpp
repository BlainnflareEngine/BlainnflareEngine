//
// Created by gorev on 20.11.2025.
//

#include "scene/SceneMeta.h"

#include "Engine.h"
#include "FileSystemUtils.h"

#include <fstream>
#include <qfileinfo.h>

namespace editor
{

SceneMeta::SceneMeta(const QString &relativeScenePath)
    : m_filePath(QString::fromStdString((Blainn::Engine::GetContentDirectory() / ToString(relativeScenePath)).string())
                 + "." + formats::metaFormat)
{
    m_entityNodes.reserve(32);

    if (QFile::exists(m_filePath))
    {
        m_meta = YAML::LoadFile(ToString(m_filePath));
    }
    else
    {
        m_meta = YAML::Node(YAML::NodeType::Map);
        m_meta["Entities"] = YAML::Node(YAML::NodeType::Sequence);
    }

    for (const auto &entityNode : m_meta["Entities"])
        m_entityNodes.emplace_back(entityNode);
}


SceneMeta::~SceneMeta() = default;


void SceneMeta::AddEntity(Blainn::Entity &entity, int position)
{
    Blainn::uuid ID = entity.GetUUID();

    auto it = eastl::find_if(m_entityNodes.begin(), m_entityNodes.end(),
                             [ID](const YAML::Node &node) { return node["ID"].as<std::string>() == ID.str(); });

    if (it != m_entityNodes.end())
    {
        BF_INFO("Found existing entity in scene meta, changing its order.");
        SetEntityPosition(entity, position);
        return;
    }

    YAML::Node entityNode;
    entityNode["ID"] = ID.str();
    entityNode["Position"] = position;
    // TODO: components

    if (position < 0 || position > m_entityNodes.size()) position = m_entityNodes.size();

    m_entityNodes.insert(m_entityNodes.begin() + position, entityNode);
}


void SceneMeta::RemoveEntity(Blainn::uuid ID)
{
    auto it = eastl::find_if(m_entityNodes.begin(), m_entityNodes.end(),
                             [ID](const YAML::Node &node) { return node["ID"].as<std::string>() == ID.str(); });

    if (it != m_entityNodes.end())
    {
        m_entityNodes.erase(it);
    }
}


void SceneMeta::SetEntityPosition(Blainn::Entity &entity, int position)
{
    RemoveEntity(entity.GetUUID());
    AddEntity(entity, position);
}


void SceneMeta::Save()
{
    YAML::Node entitiesNode(YAML::NodeType::Sequence);
    for (const auto &entityNode : m_entityNodes)
        entitiesNode.push_back(entityNode);

    m_meta["Entities"] = entitiesNode;

    std::ofstream fout(ToString(m_filePath));
    fout << m_meta;
}


void SceneMeta::GetEditorOrder(eastl::vector<Blainn::uuid> &vector) const
{
    vector.clear();

    if (!m_entityNodes.empty())
    {
        vector.reserve(m_entityNodes.size());
        for (const auto &entityNode : m_entityNodes)
        {
            std::string uuidStr = entityNode["ID"].as<std::string>();
            vector.emplace_back(uuidStr);
        }
    }
}


int SceneMeta::GetPositionInEditorOrder(const Blainn::uuid &entityID) const
{
    eastl::vector<Blainn::uuid> vector = {};
    GetEditorOrder(vector);
    auto it = eastl::find(vector.begin(), vector.end(), entityID);
    return it != vector.end() ? std::distance(vector.begin(), it) : -1;
}


/*int SceneMeta::GetEntityPosition(Blainn::Entity &entity) const
{
    Blainn::uuid ID = entity.GetUUID();
    auto it = eastl::find_if(m_entityNodes.begin(), m_entityNodes.end(),
                             [ID](const YAML::Node &node) { return node["ID"].as<std::string>() == ID.str(); });

    if (it != m_entityNodes.end())
    {
        return eastl::distance(m_entityNodes.begin(), it);
    }
    return -1;
}*/


} // namespace editor