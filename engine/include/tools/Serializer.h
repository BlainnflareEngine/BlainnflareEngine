//
// Created by gorev on 10.11.2025.
//

#pragma once
#include "AssetManager.h"
#include "Log.h"
#include "components/MeshComponent.h"
#include "scene/Entity.h"
#include "yaml-cpp/emitter.h"

namespace Blainn
{
class Serializer
{
public:
    static void Default(Entity &entity, YAML::Emitter &out)
    {
        out << YAML::Key << "Name" << YAML::Value << entity.Name().c_str();
        out << YAML::Key << "EntityID" << YAML::Value << entity.GetUUID().str();
    }

    static void Tag(Entity &entity, YAML::Emitter &out)
    {
        if (!entity.HasComponent<TagComponent>()) return;

        auto &tag = entity.GetComponent<TagComponent>();
        out << YAML::Key << "TagComponent" << YAML::Value << YAML::BeginMap;
        out << YAML::Key << "Tag" << YAML::Value << tag.Tag.c_str();
        out << YAML::EndMap;
    }

    static void Transform(Entity &entity, YAML::Emitter &out)
    {
        if (!entity.HasComponent<TransformComponent>()) return;

        auto &transform = entity.GetComponent<TransformComponent>();

        out << YAML::Key << "TransformComponent" << YAML::Value << YAML::BeginMap;

        out << YAML::Key << "Translation" << YAML::Value << YAML::BeginMap;
        out << YAML::Key << "x" << YAML::Value << transform.Translation.x;
        out << YAML::Key << "y" << YAML::Value << transform.Translation.y;
        out << YAML::Key << "z" << YAML::Value << transform.Translation.z;
        out << YAML::EndMap;

        out << YAML::Key << "Rotation" << YAML::Value << YAML::BeginMap;
        out << YAML::Key << "x" << YAML::Value << transform.GetRotation().x;
        out << YAML::Key << "y" << YAML::Value << transform.GetRotation().y;
        out << YAML::Key << "z" << YAML::Value << transform.GetRotation().z;
        out << YAML::EndMap;

        out << YAML::Key << "Scale" << YAML::Value << YAML::BeginMap;
        out << YAML::Key << "x" << YAML::Value << transform.Scale.x;
        out << YAML::Key << "y" << YAML::Value << transform.Scale.y;
        out << YAML::Key << "z" << YAML::Value << transform.Scale.z;
        out << YAML::EndMap;

        out << YAML::EndMap;
    }

    static void Relationship(Entity &entity, YAML::Emitter &out)
    {
        if (!entity.HasComponent<RelationshipComponent>()) return;

        auto &relationship = entity.GetComponent<RelationshipComponent>();
        out << YAML::Key << "RelationshipComponent" << YAML::Value << YAML::BeginMap;

        uuid parentUUID = entity.GetParentUUID();
        out << YAML::Key << "Parent" << YAML::Value << parentUUID.str();

        out << YAML::Key << "Children" << YAML::Value << YAML::BeginSeq;
        for (const auto &childID : relationship.Children)
            out << childID.str();

        out << YAML::EndSeq;
        out << YAML::EndMap;
    }

    static void Scripting(Entity &entity, YAML::Emitter &out)
    {
        if (!entity.HasComponent<ScriptingComponent>()) return;

        const auto &scripting = entity.GetComponent<ScriptingComponent>();
        out << YAML::Key << "ScriptingComponent" << YAML::Value << YAML::BeginMap;
        out << YAML::Key << "Scripts" << YAML::Value << YAML::BeginSeq;

        for (const auto &[path, info] : scripting.scriptPaths)
        {
            out << YAML::BeginMap;
            out << YAML::Key << "Path" << YAML::Value << path.c_str();
            out << YAML::Key << "ShouldTriggerStart" << YAML::Value << info.shouldTriggerStart;
            out << YAML::EndMap;
        }

        out << YAML::EndSeq;
        out << YAML::EndMap;
    }

    static void Mesh(Entity &entity, YAML::Emitter &out)
    {
        if (!entity.HasComponent<MeshComponent>()) return;

        auto &mesh = entity.GetComponent<MeshComponent>();

        out << YAML::Key << "MeshComponent" << YAML::Value << YAML::BeginMap;
        auto a = AssetManager::GetInstance().GetMeshPath(*mesh.m_meshHandle).string();
        out << YAML::Key << "Path" << YAML::Value
            << AssetManager::GetInstance().GetMeshPath(*mesh.m_meshHandle).string();
        out << YAML::EndMap;
    }
};
} // namespace Blainn
