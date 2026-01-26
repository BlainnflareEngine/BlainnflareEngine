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
    return uuid(node["EntityID"].as<std::string>());
}

inline eastl::string GetTag(const YAML::Node &node)
{
    eastl::string tag = eastl::string();
    if (node && node["Tag"]) tag = node["Tag"].as<std::string>().c_str();

    return tag;
}

inline bool HasTransform(const YAML::Node &node)
{
    if (!node || node.IsNull()) return false;

    if (node["TransformComponent"]) return true;

    return false;
}

inline bool HasTag(const YAML::Node &node)
{
    return node["TagComponent"] && node["TagComponent"]["Tag"];
}

inline TransformComponent GetTransform(const YAML::Node &node)
{
    TransformComponent transform;

    if (!node || node.IsNull())
    {
        BF_ERROR("Failed to parse transform component. Not found in .scene!");
        return transform;
    }

    auto &transformNode = node["TransformComponent"];
    if (!transformNode)
    {
        BF_ERROR("Failed to parse transform component. Not found in .scene!");
        return transform;
    }

    if (transformNode["Translation"])
    {
        auto translationNode = transformNode["Translation"];
        if (translationNode["x"] && translationNode["y"] && translationNode["z"])
        {
            Vec3 translation(translationNode["x"].as<float>(), translationNode["y"].as<float>(),
                             translationNode["z"].as<float>());
            transform.SetTranslation(translation);
        }
    }
    if (transformNode["Rotation"])
    {
        auto rotationNode = transformNode["Rotation"];
        if (rotationNode["x"] && rotationNode["y"] && rotationNode["z"])
        {
            float x = rotationNode["x"].as<float>();
            float y = rotationNode["y"].as<float>();
            float z = rotationNode["z"].as<float>();
            transform.SetRotation(Quat::CreateFromYawPitchRoll(y, x, z));
        }
    }

    if (transformNode["Scale"])
    {
        auto scaleNode = transformNode["Scale"];
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

    auto &meshNode = node["MeshComponent"];
    if (!meshNode)
    {
        BF_ERROR("Failed to parse mesh component. Not found in .scene!");
        return mesh;
    }

    Path relativeMeshPath;
    Path absolutMeshPath;
    Path relativeMaterialPath;
    Path absolutMaterialPath;

    if (meshNode["Enabled"]) mesh.Enabled = meshNode["Enabled"].as<bool>();
    else mesh.Enabled = true;

    if (meshNode["IsWalkable"]) mesh.IsWalkable = meshNode["IsWalkable"].as<bool>();
    else mesh.IsWalkable = false;


    if (meshNode["Material"])
    {
        relativeMaterialPath = meshNode["Material"].as<std::string>();
        absolutMaterialPath = Engine::GetContentDirectory() / relativeMaterialPath;
    }

    if (meshNode["Path"])
    {
        relativeMeshPath = meshNode["Path"].as<std::string>();
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

    auto &relNode = node["RelationshipComponent"];
    if (!relNode)
    {
        BF_ERROR("Failed to parse relationship component. Not found in .scene!");
        return relationship;
    }

    if (relNode["Parent"])
    {
        std::string parentIdStr = relNode["Parent"].as<std::string>();
        relationship.ParentHandle = uuid(parentIdStr);
    }

    if (const YAML::Node &childrenNode = relNode["Children"])
    {
        if (childrenNode.IsSequence())
        {
            for (const auto &childNode : childrenNode)
            {
                std::string childIdStr = childNode.as<std::string>("");
                relationship.Children.push_back(uuid(childIdStr));
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
        BF_ERROR("Scripting component not found or invalid in .scene file.");
        return component;
    }

    auto &scriptingNode = node["ScriptingComponent"];
    if (!scriptingNode)
    {
        BF_ERROR("Failed to parse script component. Not found in .scene!");
        return component;
    }

    const auto &scriptsNode = scriptingNode["Scripts"];
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
    if (!node || node.IsNull()) return;

    auto &physNode = node["PhysicsComponent"];
    if (!physNode)
    {
        BF_ERROR("Failed to parse physics component. Not found in .scene!");
        return;
    }

    ComponentShapeType shapeType = static_cast<ComponentShapeType>(physNode["ShapeType"].as<int>());
    PhysicsComponentMotionType motionType = static_cast<PhysicsComponentMotionType>(physNode["MotionType"].as<int>());
    float gravityFactor = physNode["GravityFactor"].as<float>();
    bool isTrigger = physNode["IsTrigger"].as<bool>();
    ObjectLayer layer = static_cast<ObjectLayer>(physNode["ObjectLayer"].as<int>());
    uint8_t constraints = static_cast<uint8_t>(AllowedDOFs::All);

    if (physNode["Constraints"])
    {
        constraints = static_cast<uint8_t>(physNode["Constraints"].as<int>());
    }

    ShapeCreationSettings shapeSettings(shapeType);

    if (auto &shapeSettingsNode = physNode["ShapeSettings"])
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
                Vec3(extents["X"].as<float>(), extents["Y"].as<float>(), extents["Z"].as<float>());
        }
    }

    PhysicsComponentSettings settings(entity, shapeType);
    settings.gravityFactor = gravityFactor;
    settings.isTrigger = isTrigger;
    settings.layer = layer;
    settings.motionType = motionType;
    settings.shapeSettings = shapeSettings;
    settings.allowedDOFs = static_cast<AllowedDOFs>(constraints);
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
        BF_ERROR("AIController component not found or invalid in .scene file.");
        return;
    }

    auto &aiControllerNode = node["AIControllerComponent"];
    if (!aiControllerNode)
    {
        BF_ERROR("Failed to parse AI controller component. Not found in .scene!");
        return;
    }

    std::string path = aiControllerNode["Path"].as<std::string>("");
    float movementSpeed = aiControllerNode["MovementSpeed"].as<float>();
    float stoppingDistance = aiControllerNode["StoppingDistance"].as<float>();
    float groundOffset = 0.5;

    if (aiControllerNode["GroundOffset"]) groundOffset = aiControllerNode["GroundOffset"].as<float>();

    AISubsystem::GetInstance().CreateAttachAIControllerComponent(entity, path);
    auto &comp = entity.GetComponent<AIControllerComponent>();
    comp.MovementSpeed = movementSpeed;
    comp.StoppingDistance = stoppingDistance;
    comp.GroundOffset = groundOffset;
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
        BF_ERROR("Camera component not found or invalid in .scene file.");
        return camera;
    }

    auto &camNode = node["CameraComponent"];
    if (!camNode)
    {
        BF_ERROR("Camera component not found.");
        return camera;
    }

    if (camNode["Priority"]) camera.CameraPriority = camNode["Priority"].as<int32_t>(0);
    camera.camera.Reset(75.f, 16.f / 9.f, 0.01, 1000);

    if (auto &settings = camNode["CameraSettings"])
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
        BF_ERROR("Skybox component not found or invalid in .scene file.");
        return component;
    }

    auto &skyboxNode = node["SkyboxComponent"];
    if (!skyboxNode)
    {
        BF_ERROR("Skybox component not found or invalid in .scene file.");
        return component;
    }

    Path relativePath;
    Path absolutPath;

    relativePath = skyboxNode["Path"].as<std::string>();
    absolutPath = Engine::GetContentDirectory() / relativePath;

    if (std::filesystem::is_regular_file(absolutPath))
    {
        component.textureHandle = AssetManager::GetInstance().HasTexture(relativePath)
                                      ? AssetManager::GetInstance().GetTexture(relativePath)
                                      : AssetManager::GetInstance().LoadTexture(relativePath, TextureType::CUBEMAP);
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

    if (!node || node.IsNull())
    {
        BF_ERROR("Navmesh Volume component not found or invalid in .scene file.");
        return component;
    }

    auto &volumeNode = node["NavmeshVolumeComponent"];
    if (!volumeNode)
    {
        BF_ERROR("Navmesh Volume component not found or invalid in .scene file.");
        return component;
    }

    Vec3 extents = {volumeNode["Extent"]["X"].as<float>(), volumeNode["Extent"]["Y"].as<float>(),
                    volumeNode["Extent"]["Z"].as<float>()};
    component.LocalBounds =
        JPH::AABox::sFromTwoPoints({-extents.x, -extents.y, -extents.z}, {extents.x, extents.y, extents.z});
    component.IsEnabled = volumeNode["IsEnabled"].as<bool>(true);
    component.CellSize = volumeNode["CellSize"].as<float>();
    component.AgentHeight = volumeNode["AgentHeight"].as<float>();
    component.AgentRadius = volumeNode["AgentRadius"].as<float>();
    component.AgentMaxClimb = volumeNode["AgentMaxClimb"].as<float>();
    component.AgentMaxSlope = volumeNode["AgentMaxSlope"].as<float>();
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
        BF_ERROR("Stimulus component not found or invalid in .scene file.");
        return component;
    }

    auto &stimulusNode = node["StimulusComponent"];
    if (!stimulusNode)
    {
        BF_ERROR("Stimulus component not found in .scene file.");
        return component;
    }

    component.enableSight = stimulusNode["EnableSight"].as<bool>();
    component.enableSound = stimulusNode["EnableSound"].as<bool>();
    component.enableTouch = stimulusNode["EnableTouch"].as<bool>();
    component.enableDamage = stimulusNode["EnableDamage"].as<bool>();

    component.sightRadius = stimulusNode["SightRadius"].as<float>();
    component.soundRadius = stimulusNode["SoundRadius"].as<float>();
    component.tag = stimulusNode["Tag"].as<std::string>("").c_str();
    component.enabled = stimulusNode["Enabled"].as<bool>();

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

    auto &perceptionNode = node["PerceptionComponent"];
    if (!perceptionNode)
    {
        BF_ERROR("Perception component not found in .scene!");
        return perception;
    }

    if (perceptionNode["EnableSight"]) perception.enableSight = perceptionNode["EnableSight"].as<bool>();
    if (perceptionNode["SightRange"]) perception.sightRange = perceptionNode["SightRange"].as<float>();
    if (perceptionNode["SightFOV"]) perception.sightFOV = perceptionNode["SightFOV"].as<float>();
    if (perceptionNode["SightForgetTime"]) perception.sightForgetTime = perceptionNode["SightForgetTime"].as<float>();
    if (perceptionNode["SightLOSCheckInterval"])
        perception.sightLOSCheckInterval = perceptionNode["SightLOSCheckInterval"].as<float>();
    if (perceptionNode["SightRequireLOS"]) perception.sightRequireLOS = perceptionNode["SightRequireLOS"].as<bool>();

    if (perceptionNode["EnableSound"]) perception.enableSound = perceptionNode["EnableSound"].as<bool>();
    if (perceptionNode["SoundRange"]) perception.soundRange = perceptionNode["SoundRange"].as<float>();
    if (perceptionNode["SoundForgetTime"]) perception.soundForgetTime = perceptionNode["SoundForgetTime"].as<float>();
    if (perceptionNode["SoundMinStrength"])
        perception.soundMinStrength = perceptionNode["SoundMinStrength"].as<float>();

    if (perceptionNode["EnableTouch"]) perception.enableTouch = perceptionNode["EnableTouch"].as<bool>();
    if (perceptionNode["TouchForgetTime"]) perception.touchForgetTime = perceptionNode["TouchForgetTime"].as<float>();

    if (perceptionNode["EnableDamage"]) perception.enableDamage = perceptionNode["EnableDamage"].as<bool>();
    if (perceptionNode["DamageForgetTime"])
        perception.damageForgetTime = perceptionNode["DamageForgetTime"].as<float>();

    if (perceptionNode["UpdateInterval"]) perception.updateInterval = perceptionNode["UpdateInterval"].as<float>();
    if (perceptionNode["MaxUpdateDistance"])
        perception.maxUpdateDistance = perceptionNode["MaxUpdateDistance"].as<float>();
    if (perceptionNode["Enabled"]) perception.enabled = perceptionNode["Enabled"].as<bool>();

    if (const YAML::Node &ignoreTagsNode = perceptionNode["IgnoreTags"])
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

    if (const YAML::Node &priorityTagsNode = perceptionNode["PriorityTags"])
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

inline bool HasDirectionalLight(const YAML::Node &node)
{
    if (!node || node.IsNull()) return false;

    if (node["DirectionalLightComponent"]) return true;

    return false;
}

inline DirectionalLightComponent GetDirectionalLight(const YAML::Node &node)
{
    DirectionalLightComponent directionalLight;

    auto &lightNode = node["DirectionalLightComponent"];
    if (!lightNode)
    {
        BF_ERROR("Failed to parse directional light component. Not found in .scene!");
        return directionalLight;
    }

    directionalLight.Color = {lightNode["Color"]["R"].as<float>(), lightNode["Color"]["G"].as<float>(),
                              lightNode["Color"]["B"].as<float>(), lightNode["Color"]["A"].as<float>()};

    return directionalLight;
}

inline bool HasPointLight(const YAML::Node &node)
{
    if (!node || node.IsNull()) return false;

    if (node["PointLightComponent"]) return true;

    return false;
}

inline PointLightComponent GetPointLight(const YAML::Node &node)
{
    PointLightComponent pointLight;

    auto &lightNode = node["PointLightComponent"];
    if (!lightNode)
    {
        BF_ERROR("Failed to parse point light component. Not found in .scene!");
        return pointLight;
    }

    pointLight.Color = {lightNode["Color"]["R"].as<float>(), lightNode["Color"]["G"].as<float>(),
                        lightNode["Color"]["B"].as<float>(), lightNode["Color"]["A"].as<float>()};

    pointLight.FalloffStart = lightNode["FalloffStart"].as<float>();
    pointLight.FalloffEnd = lightNode["FalloffEnd"].as<float>();

    pointLight.Intensity = lightNode["Intensity"].as<float>(1);

    return pointLight;
}

inline bool HasSpotLight(const YAML::Node &node)
{
    if (!node || node.IsNull()) return false;

    if (node["SpotLightComponent"]) return true;

    return false;
}

inline SpotLightComponent GetSpotLight(const YAML::Node &node)
{
    SpotLightComponent pointLight;

    auto &lightNode = node["SpotLightComponent"];
    if (!lightNode)
    {
        BF_ERROR("Failed to parse spotlight component. Not found in .scene!");
        return pointLight;
    }

    pointLight.Color = {lightNode["Color"]["R"].as<float>(), lightNode["Color"]["G"].as<float>(),
                        lightNode["Color"]["B"].as<float>(), lightNode["Color"]["A"].as<float>()};

    pointLight.FalloffStart = lightNode["FalloffStart"].as<float>();
    pointLight.FalloffEnd = lightNode["FalloffEnd"].as<float>();

    pointLight.Intensity = lightNode["Intensity"].as<float>(1);
    pointLight.SpotInnerAngle = lightNode["InnerAngle"].as<float>(1);
    pointLight.SpotOuterAngle = lightNode["OuterAngle"].as<float>(1);

    return pointLight;
}
} // namespace Blainn
