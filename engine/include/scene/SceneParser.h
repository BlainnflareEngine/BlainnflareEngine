//
// Created by gorev on 13.11.2025.
//

#pragma once
#include "aliases.h"
#include "pch.h"

#include "components/ScriptingComponent.h"
#include "components/CameraComponent.h"
#include "components/MeshComponent.h"
#include "physics/BodyBuilder.h"
#include "Engine.h"

namespace Blainn
{
inline uuid GetID(const YAML::Node &node)
{
    return uuid::fromStrFactory(node["EntityID"].as<std::string>().c_str());
}

inline eastl::string GetTag(const YAML::Node &node)
{
    eastl::string tag = eastl::string();
    if (node["TagComponent"] && node["TagComponent"]["Tag"])
        tag = node["TagComponent"]["Tag"].as<std::string>().c_str();

    return tag;
}

inline bool HasTransform(const YAML::Node &node)
{
    if (!node || node.IsNull()) return false;

    if (node["TransformComponent"]) return true;

    return false;
}

inline TransformComponent GetTransform(const YAML::Node &node)
{
    TransformComponent transform;

    if (!node || node.IsNull())
    {
        BF_ERROR("Failed to parse transform component. Not found in .scene!");
        return transform;
    }

    if (node["Translation"])
    {
        auto translationNode = node["Translation"];
        if (translationNode["x"] && translationNode["y"] && translationNode["z"])
        {
            Vec3 translation(translationNode["x"].as<float>(), translationNode["y"].as<float>(),
                             translationNode["z"].as<float>());
            transform.SetTranslation(translation);
        }
    }
    if (node["Rotation"])
    {
        auto rotationNode = node["Rotation"];
        if (rotationNode["x"] && rotationNode["y"] && rotationNode["z"])
        {
            float x = rotationNode["x"].as<float>();
            float y = rotationNode["y"].as<float>();
            float z = rotationNode["z"].as<float>();
            transform.SetRotationEuler(Vec3(x, y, z));
        }
    }

    if (node["Scale"])
    {
        auto scaleNode = node["Scale"];
        if (scaleNode["x"] && scaleNode["y"] && scaleNode["z"])
        {
            Vec3 scale(scaleNode["x"].as<float>(), scaleNode["y"].as<float>(), scaleNode["z"].as<float>());
            transform.SetScale(scale);
        }
    }

    return transform;
}

inline bool HasMesh(const YAML::Node &node)
{
    if (!node || node.IsNull()) return false;

    if (node["MeshComponent"]) return true;

    return false;
}


inline MeshComponent GetMesh(const YAML::Node &node)
{
    MeshComponent mesh = MeshComponent(AssetManager::GetDefaultMesh());

    if (!node || node.IsNull())
    {
        BF_ERROR("Failed to parse transform component. Not found in .scene!");
        return mesh;
    }

    Path relativeMeshPath;
    Path absolutMeshPath;
    Path relativeMaterialPath;
    Path absolutMaterialPath;

    if (node["Material"])
    {
        relativeMaterialPath = node["Material"].as<std::string>();
        absolutMaterialPath = Engine::GetContentDirectory() / relativeMaterialPath;
    }

    if (node["Path"])
    {
        relativeMeshPath = node["Path"].as<std::string>();
        absolutMeshPath = Engine::GetContentDirectory() / relativeMeshPath;
    }

    if (!std::filesystem::is_regular_file(absolutMeshPath))
    {
        mesh.MeshHandle = AssetManager::GetInstance().GetDefaultMesh();
    }
    else
    {
        mesh.MeshHandle =
            AssetManager::GetInstance().HasMesh(relativeMeshPath)
                ? AssetManager::GetInstance().GetMesh(relativeMeshPath)
                : AssetManager::GetInstance().LoadMesh(relativeMeshPath, ImportMeshData::GetMeshData(absolutMeshPath));
    }

    if (!std::filesystem::is_regular_file(absolutMaterialPath))
    {
        mesh.MaterialHandle = AssetManager::GetInstance().GetDefaultMaterialHandle();
    }
    else
    {
        mesh.MaterialHandle = AssetManager::GetInstance().HasMaterial(relativeMaterialPath)
                                  ? AssetManager::GetInstance().GetMaterial(relativeMaterialPath)
                                  : AssetManager::GetInstance().LoadMaterial(relativeMaterialPath);
    }

    return mesh;
}

inline bool HasRelationship(const YAML::Node &node)
{
    if (!node || node.IsNull()) return false;

    if (node["RelationshipComponent"]) return true;

    return false;
}

inline RelationshipComponent GetRelationship(const YAML::Node &node)
{
    RelationshipComponent relationship;

    if (!node || node.IsNull())
    {
        BF_ERROR("Failed to parse relationship component. Not found in .scene!");
        return relationship;
    }

    if (node["Parent"])
    {
        std::string parentIdStr = node["Parent"].as<std::string>();
        relationship.ParentHandle = uuid::fromStrFactory(parentIdStr);
    }

    if (const YAML::Node &childrenNode = node["Children"])
    {
        if (childrenNode.IsSequence())
        {
            for (const auto &childNode : childrenNode)
            {
                std::string childIdStr = childNode.as<std::string>("");
                relationship.Children.push_back(uuid::fromStrFactory(childIdStr));
            }
        }
    }

    return relationship;
}

inline bool HasScripting(const YAML::Node &node)
{
    if (!node || node.IsNull()) return false;

    if (node["ScriptingComponent"]) return true;

    return false;
}

inline ScriptingComponent GetScripting(const YAML::Node &node)
{
    ScriptingComponent component;

    if (!node || node.IsNull() || !node.IsMap())
    {
        BF_WARN("Scripting component not found or invalid in .scene file.");
        return component;
    }

    const YAML::Node &scriptsNode = node["Scripts"];
    if (!scriptsNode || !scriptsNode.IsSequence()) return component;

    for (const auto &scriptNode : scriptsNode)
    {
        if (!scriptNode.IsMap()) continue;

        std::string path = scriptNode["Path"].as<std::string>("");
        if (path.empty()) continue;

        bool shouldTriggerStart = true;
        if (const YAML::Node &triggerNode = scriptNode["ShouldTriggerStart"])
        {
            shouldTriggerStart = triggerNode.as<bool>(true);
        }

        component.scriptPaths[eastl::string(path.c_str())] = ScriptInfo{shouldTriggerStart};
    }

    return component;
}

inline bool HasPhysics(const YAML::Node &node)
{
    if (!node || node.IsNull()) return false;

    if (node["PhysicsComponent"]) return true;

    return false;
}

inline PhysicsComponent GetPhysics(const YAML::Node &node)
{
    PhysicsComponent physics;

    if (!node || node.IsNull()) return physics;


    physics.parentId = uuid::fromStrFactory(node["ParentID"].as<std::string>());
    ComponentShapeType shapeType = static_cast<ComponentShapeType>(node["ShapeType"].as<int>());
    PhysicsComponentMotionType motionType = static_cast<PhysicsComponentMotionType>(node["MotionType"].as<int>());
    float gravityFactor = node["GravityFactor"].as<float>();
    bool isTrigger = node["IsTrigger"].as<bool>();
    ObjectLayer layer = static_cast<ObjectLayer>(node["ObjectLayer"].as<int>());

    ShapeCreationSettings shapeSettings(shapeType);
    eastl::optional<ShapeHierarchy> createdShapeHierarchy = ShapeFactory::CreateShape(shapeSettings);

    if (!createdShapeHierarchy.has_value())
    {
        BF_ERROR("Error in creating shape for physics component");
        return physics;
    }

    physics.UpdateShape(shapeType, createdShapeHierarchy.value());

    auto transform = GetTransform(node["TransformComponent"]);

    BodyBuilder builder;
    builder.SetMotionType(motionType)
        .SetPosition(transform.GetTranslation())
        .SetRotation(transform.GetRotation())
        .SetShape(createdShapeHierarchy.value().shapePtr)
        .SetIsTrigger(isTrigger)
        .SetGravityFactor(gravityFactor)
        .SetLayer(layer);

    physics.bodyId = builder.Build();
    PhysicsSubsystem::AddBodyConnection(physics.bodyId, physics.parentId);

    return physics;
}

inline bool HasCamera(const YAML::Node &node)
{
    if (!node || node.IsNull()) return false;

    if (node["CameraComponent"]) return true;

    return false;
}

inline CameraComponent GetCamera(const YAML::Node &node)
{
    CameraComponent camera;
    if (!node || node.IsNull() || !node.IsMap())
    {
        BF_WARN("Camera component not found or invalid in .scene file.");
        return camera;
    }
    if (node["IsActiveCamera"]) camera.IsActiveCamera = node["IsActiveCamera"].as<bool>();
    camera.camera.Reset(75.f, 16.f / 9.f, 0.01, 1000);

    if (auto &settings = node["CameraSettings"])
    {
        // TODO: other settigns
        camera.camera.SetFovDegrees(settings["FOV"].as<float>());
        camera.camera.SetNearZ(settings["NearZ"].as<float>());
        camera.camera.SetFarZ(settings["FarZ"].as<float>());
    }

    return camera;
}
} // namespace Blainn
