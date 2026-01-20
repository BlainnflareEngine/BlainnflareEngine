//
// Created by gorev on 13.11.2025.
//

#pragma once
#include "AISubsystem.h"
#include "aliases.h"
#include "pch.h"

#include "components/ScriptingComponent.h"
#include "components/CameraComponent.h"
#include "components/MeshComponent.h"
#include "components/SkyboxComponent.h"
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

inline void GetPhysics(const YAML::Node &node, const Entity &entity)
{
    ComponentShapeType shapeType = static_cast<ComponentShapeType>(node["ShapeType"].as<int>());
    PhysicsComponentMotionType motionType = static_cast<PhysicsComponentMotionType>(node["MotionType"].as<int>());
    float gravityFactor = node["GravityFactor"].as<float>();
    bool isTrigger = node["IsTrigger"].as<bool>();
    ObjectLayer layer = static_cast<ObjectLayer>(node["ObjectLayer"].as<int>());

    ShapeCreationSettings shapeSettings(shapeType);

    if (auto &shapeSettingsNode = node["ShapeSettings"])
    {
        if (shapeSettingsNode["HalfHeight"])
        {
            shapeSettings.halfCylinderHeight = shapeSettingsNode["HalfHeight"].as<float>();
        }

        if (shapeSettingsNode["Radius"])
        {
            shapeSettings.radius = shapeSettingsNode["Radius"].as<float>();
        }

        if (auto &extents = shapeSettingsNode["HalfExtent"])
        {
            shapeSettings.halfExtents =
                Blainn::Vec3(extents["X"].as<float>(), extents["Y"].as<float>(), extents["Z"].as<float>());
        }
    }

    PhysicsComponentSettings settings(entity, shapeType);
    settings.gravityFactor = gravityFactor;
    settings.isTrigger = isTrigger;
    settings.layer = layer;
    settings.motionType = motionType;
    settings.shapeSettings = shapeSettings;
    PhysicsSubsystem::CreateAttachPhysicsComponent(settings);
}

inline bool HasAIController(const YAML::Node &node)
{
    if (!node || node.IsNull()) return false;

    if (node["AIControllerComponent"]) return true;

    return false;
}

inline void GetAIController(const YAML::Node &node, const Entity &entity)
{
    if (!node || node.IsNull() || !node.IsMap())
    {
        BF_WARN("AIController component not found or invalid in .scene file.");
        return;
    }

    std::string path = node["Path"].as<std::string>("");
    float movementSpeed = node["MovementSpeed"].as<float>();
    float stoppingDistance = node["StoppingDistance"].as<float>();

    AISubsystem::GetInstance().CreateAttachAIControllerComponent(entity, path);
    auto &comp = entity.GetComponent<AIControllerComponent>();
    comp.MovementSpeed = movementSpeed;
    comp.StoppingDistance = stoppingDistance;
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
    if (node["Priority"]) camera.CameraPriority = node["Priority"].as<int32_t>(0);
    camera.camera.Reset(75.f, 16.f / 9.f, 0.01, 1000);

    if (auto &settings = node["CameraSettings"])
    {
        camera.camera.SetFovDegrees(settings["FOV"].as<float>());
        camera.camera.SetNearZ(settings["NearZ"].as<float>());
        camera.camera.SetFarZ(settings["FarZ"].as<float>());
    }

    return camera;
}

inline bool HasSkybox(const YAML::Node &node)
{
    if (!node || node.IsNull()) return false;

    if (node["SkyboxComponent"]) return true;

    return false;
}

inline SkyboxComponent GetSkybox(const YAML::Node &node)
{
    SkyboxComponent component;

    if (!node || node.IsNull() || !node.IsMap())
    {
        BF_WARN("Skybox component not found or invalid in .scene file.");
        return component;
    }

    Path relativePath;
    Path absolutPath;

    relativePath = node["Path"].as<std::string>();
    absolutPath = Engine::GetContentDirectory() / relativePath;

    if (std::filesystem::is_regular_file(absolutPath))
    {
        component.textureHandle = AssetManager::GetInstance().HasTexture(relativePath)
                                      ? AssetManager::GetInstance().GetTexture(relativePath)
                                      : AssetManager::GetInstance().LoadTexture(relativePath, TextureType::ALBEDO);
    }

    return component;
}

inline bool HasNavMeshVolume(const YAML::Node &node)
{
    if (!node || node.IsNull()) return false;

    if (node["NavmeshVolumeComponent"]) return true;

    return false;
}

inline NavmeshVolumeComponent GetNavMeshVolume(const YAML::Node &node)
{
    NavmeshVolumeComponent component;

    if (!node || node.IsNull() || !node.IsMap())
    {
        BF_WARN("Navmesh Volume component not found or invalid in .scene file.");
        return component;
    }

    Vec3 extents = {node["Extent"]["X"].as<float>(), node["Extent"]["Y"].as<float>(), node["Extent"]["Z"].as<float>()};
    component.LocalBounds =
        JPH::AABox::sFromTwoPoints({-extents.x, -extents.y, -extents.z}, {extents.x, extents.y, extents.z});
    component.IsEnabled = node["IsEnabled"].as<bool>();
    component.CellSize = node["CellSize"].as<float>();
    component.AgentHeight = node["AgentHeight"].as<float>();
    component.AgentRadius = node["AgentRadius"].as<float>();
    component.AgentMaxClimb = node["AgentMaxClimb"].as<float>();
    component.AgentMaxSlope = node["AgentMaxSlope"].as<float>();
    return component;
}

inline std::string NavMeshData(const YAML::Node &node)
{
    if (!node || node.IsNull()) return "";

    if (!node["NavMeshData"] || !node["NavMeshData"]["Path"]) return "";

    return node["NavMeshData"]["Path"].as<std::string>();
}

inline bool HasStimulus(const YAML::Node &node)
{
    if (!node || node.IsNull()) return false;

    if (node["StimulusComponent"]) return true;

    return false;
}

inline StimulusComponent GetStimulus(const YAML::Node &node)
{
    StimulusComponent component;

    if (!node || node.IsNull() || !node.IsMap())
    {
        BF_WARN("Stimulus component not found or invalid in .scene file.");
        return component;
    }

    component.enableSight = node["EnableSight"].as<bool>();
    component.enableSound = node["EnableSound"].as<bool>();
    component.enableTouch = node["EnableTouch"].as<bool>();
    component.enableDamage = node["EnableDamage"].as<bool>();

    component.sightRadius = node["SightRadius"].as<float>();
    component.soundRadius = node["SoundRadius"].as<float>();
    component.tag = node["Tag"].as<std::string>("").c_str();
    component.enabled = node["Enabled"].as<bool>();

    return component;
}

inline bool HasPerception(const YAML::Node &node)
{
    if (!node || node.IsNull()) return false;

    if (node["PerceptionComponent"]) return true;

    return false;
}

inline PerceptionComponent GetPerception(const YAML::Node &node)
{
    PerceptionComponent perception;

    if (!node || node.IsNull())
    {
        BF_ERROR("Failed to parse perception component. Not found in .scene!");
        return perception;
    }

    if (node["EnableSight"]) perception.enableSight = node["EnableSight"].as<bool>();
    if (node["SightRange"]) perception.sightRange = node["SightRange"].as<float>();
    if (node["SightFOV"]) perception.sightFOV = node["SightFOV"].as<float>();
    if (node["SightForgetTime"]) perception.sightForgetTime = node["SightForgetTime"].as<float>();
    if (node["SightLOSCheckInterval"])
        perception.sightLOSCheckInterval = node["SightLOSCheckInterval"].as<float>();
    if (node["SightRequireLOS"]) perception.sightRequireLOS = node["SightRequireLOS"].as<bool>();

    if (node["EnableSound"]) perception.enableSound = node["EnableSound"].as<bool>();
    if (node["SoundRange"]) perception.soundRange = node["SoundRange"].as<float>();
    if (node["SoundForgetTime"]) perception.soundForgetTime = node["SoundForgetTime"].as<float>();
    if (node["SoundMinStrength"])
        perception.soundMinStrength = node["SoundMinStrength"].as<float>();

    if (node["EnableTouch"]) perception.enableTouch = node["EnableTouch"].as<bool>();
    if (node["TouchForgetTime"]) perception.touchForgetTime = node["TouchForgetTime"].as<float>();

    if (node["EnableDamage"]) perception.enableDamage = node["EnableDamage"].as<bool>();
    if (node["DamageForgetTime"])
        perception.damageForgetTime = node["DamageForgetTime"].as<float>();

    if (node["UpdateInterval"]) perception.updateInterval = node["UpdateInterval"].as<float>();
    if (node["MaxUpdateDistance"])
        perception.maxUpdateDistance = node["MaxUpdateDistance"].as<float>();
    if (node["Enabled"]) perception.enabled = node["Enabled"].as<bool>();

    if (const YAML::Node &ignoreTagsNode = node["IgnoreTags"])
    {
        if (ignoreTagsNode.IsSequence())
        {
            perception.ignoreTags.clear();
            for (const auto &tagNode : ignoreTagsNode)
            {
                std::string tagStr = tagNode.as<std::string>("");
                if (!tagStr.empty()) perception.ignoreTags.push_back(tagStr.c_str());
            }
        }
    }

    if (const YAML::Node &priorityTagsNode = node["PriorityTags"])
    {
        if (priorityTagsNode.IsSequence())
        {
            perception.priorityTags.clear();
            for (const auto &tagNode : priorityTagsNode)
            {
                std::string tagStr = tagNode.as<std::string>("");
                if (!tagStr.empty()) perception.priorityTags.push_back(tagStr.c_str());
            }
        }
    }

    return perception;
}
} // namespace Blainn
