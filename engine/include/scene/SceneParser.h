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
#include "components/PrefabComponent.h"
#include "components/LightComponent.h"
#include "components/NavMeshVolumeComponent.h"
#include "components/StimulusComponent.h"
#include "components/PerceptionComponent.h"
#include "components/PhysicsComponent.h"
#include "components/AIControllerComponent.h"
#include "AssetManager.h"

namespace Blainn
{
inline uuid GetID(const YAML::Node& node)
{
    if (!node || node.IsNull()) return Rand::getRandomUUID();
    return uuid::fromStrFactory(node["EntityID"].as<std::string>(Rand::getRandomUUID().str()).c_str());
}

inline bool HasTag(const YAML::Node& node, bool packed = true)
{
    if (!node || node.IsNull()) return false;
    if (packed) {
        return node["TagComponent"] && node["TagComponent"]["Tag"] && !node["TagComponent"]["Tag"].IsNull();
    } else {
        return node["Tag"] && !node["Tag"].IsNull();
    }
}

inline eastl::string GetTag(const YAML::Node& node, bool packed = true)
{
    if (!HasTag(node, packed)) return "Untagged";
    if (packed) {
        return node["TagComponent"]["Tag"].as<std::string>("Untagged").c_str();
    } else {
        return node["Tag"].as<std::string>("Untagged").c_str();
    }
}

inline bool HasTransform(const YAML::Node& node, bool packed = true)
{
    if (!node || node.IsNull()) return false;
    if (packed) {
        return node["TransformComponent"] && node["TransformComponent"].IsDefined();
    } else {
        return (node["Translation"] && node["Translation"].IsDefined()) ||
               (node["Rotation"] && node["Rotation"].IsDefined()) ||
               (node["Scale"] && node["Scale"].IsDefined());
    }
}

inline TransformComponent GetTransform(const YAML::Node& node, bool packed = true)
{
    TransformComponent transform;
    if (!node || node.IsNull()) return transform;

    YAML::Node transformNode;
    if (packed) {
        transformNode = node["TransformComponent"];
    } else {
        transformNode = node;
    }
    if (!transformNode || transformNode.IsNull()) return transform;

    if (const YAML::Node& translationNode = transformNode["Translation"])
    {
        if (translationNode["x"] && translationNode["y"] && translationNode["z"])
        {
            Vec3 translation(
                translationNode["x"].as<float>(),
                translationNode["y"].as<float>(),
                translationNode["z"].as<float>()
            );
            transform.SetTranslation(translation);
        }
    }

    if (const YAML::Node& rotationNode = transformNode["Rotation"])
    {
        if (rotationNode["x"] && rotationNode["y"] && rotationNode["z"])
        {
            float pitch = rotationNode["x"].as<float>();
            float yaw = rotationNode["y"].as<float>();
            float roll = rotationNode["z"].as<float>();
            transform.SetRotation(Quat::CreateFromYawPitchRoll(yaw, pitch, roll));
        }
    }

    if (const YAML::Node& scaleNode = transformNode["Scale"])
    {
        if (scaleNode["x"] && scaleNode["y"] && scaleNode["z"])
        {
            Vec3 scale(
                scaleNode["x"].as<float>(),
                scaleNode["y"].as<float>(),
                scaleNode["z"].as<float>()
            );
            transform.SetScale(scale);
        }
    }

    return transform;
}

inline bool HasMesh(const YAML::Node& node, bool packed = true)
{
    if (!node || node.IsNull()) return false;
    if (packed) {
        return node["MeshComponent"] && node["MeshComponent"].IsDefined();
    } else {
        return node["Path"] && !node["Path"].IsNull();
    }
}

inline MeshComponent GetMesh(const YAML::Node& node, bool packed = true)
{
    MeshComponent mesh = MeshComponent(AssetManager::GetDefaultMesh());
    if (!node || node.IsNull()) return mesh;

    YAML::Node meshNode;
    if (packed) {
        meshNode = node["MeshComponent"];
    } else {
        meshNode = node;
    }
    if (!meshNode || meshNode.IsNull()) return mesh;

    if (meshNode["Enabled"]) mesh.Enabled = meshNode["Enabled"].as<bool>();
    else mesh.Enabled = true;

    if (meshNode["IsWalkable"]) mesh.IsWalkable = meshNode["IsWalkable"].as<bool>();
    else mesh.IsWalkable = false;

    Path relativeMaterialPath;
    Path absolutMaterialPath;
    if (meshNode["Material"])
    {
        relativeMaterialPath = meshNode["Material"].as<std::string>();
        absolutMaterialPath = Engine::GetContentDirectory() / relativeMaterialPath;
    }

    Path relativeMeshPath;
    Path absolutMeshPath;
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
        mesh.MeshHandle = AssetManager::GetInstance().HasMesh(relativeMeshPath)
            ? AssetManager::GetInstance().GetMesh(relativeMeshPath)
            : AssetManager::GetInstance().LoadMesh(
                relativeMeshPath,
                ImportMeshData::GetMeshData(absolutMeshPath)
            );
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

inline bool HasCamera(const YAML::Node& node, bool packed = true)
{
    if (!node || node.IsNull()) return false;
    if (packed) {
        return node["CameraComponent"] && node["CameraComponent"].IsDefined();
    } else {
        return node["CameraSettings"] && node["CameraSettings"].IsDefined();
    }
}

inline CameraComponent GetCamera(const YAML::Node& node, bool packed = true)
{
    CameraComponent camera;
    if (!node || node.IsNull() || !node.IsMap()) return camera;

    YAML::Node camNode;
    if (packed) {
        camNode = node["CameraComponent"];
    } else {
        camNode = node;
    }
    if (!camNode || camNode.IsNull()) return camera;

    if (camNode["Priority"]) camera.CameraPriority = camNode["Priority"].as<int32_t>(0);

    camera.camera.Reset(75.f, 16.f / 9.f, 0.01f, 1000.f);

    if (const YAML::Node& settings = camNode["CameraSettings"])
    {
        if (settings["FOV"]) camera.camera.SetFovDegrees(settings["FOV"].as<float>());
        if (settings["NearZ"]) camera.camera.SetNearZ(settings["NearZ"].as<float>());
        if (settings["FarZ"]) camera.camera.SetFarZ(settings["FarZ"].as<float>());
    }

    return camera;
}

inline bool HasSkybox(const YAML::Node& node, bool packed = true)
{
    if (!node || node.IsNull()) return false;
    if (packed) {
        return node["SkyboxComponent"] && node["SkyboxComponent"].IsDefined();
    } else {
        return node["Path"] && !node["Path"].IsNull();
    }
}

inline SkyboxComponent GetSkybox(const YAML::Node& node, bool packed = true)
{
    SkyboxComponent component;
    if (!node || node.IsNull() || !node.IsMap()) return component;

    YAML::Node skyboxNode;
    if (packed) {
        skyboxNode = node["SkyboxComponent"];
    } else {
        skyboxNode = node;
    }
    if (!skyboxNode || skyboxNode.IsNull() || !skyboxNode["Path"]) return component;

    Path relativePath = skyboxNode["Path"].as<std::string>();
    Path absolutPath = Engine::GetContentDirectory() / relativePath;

    if (std::filesystem::is_regular_file(absolutPath))
    {
        component.textureHandle = AssetManager::GetInstance().HasTexture(relativePath)
            ? AssetManager::GetInstance().GetTexture(relativePath)
            : AssetManager::GetInstance().LoadTexture(relativePath, TextureType::CUBEMAP);
    }

    return component;
}

inline bool HasScripting(const YAML::Node& node, bool packed = true)
{
    if (!node || node.IsNull()) return false;
    if (packed) {
        return node["ScriptingComponent"] && node["ScriptingComponent"].IsDefined();
    } else {
        return node["Scripts"] && node["Scripts"].IsSequence();
    }
}

inline ScriptingComponent GetScripting(const YAML::Node& node, bool packed = true)
{
    ScriptingComponent component;
    if (!node || node.IsNull() || !node.IsMap()) return component;

    YAML::Node scriptingNode;
    if (packed) {
        scriptingNode = node["ScriptingComponent"];
    } else {
        scriptingNode = node;
    }
    if (!scriptingNode || scriptingNode.IsNull()) return component;

    const YAML::Node& scriptsNode = scriptingNode["Scripts"];
    if (!scriptsNode || !scriptsNode.IsSequence()) return component;

    for (const auto& scriptNode : scriptsNode)
    {
        if (!scriptNode.IsMap()) continue;

        std::string path = scriptNode["Path"].as<std::string>("");
        if (path.empty()) continue;

        bool shouldTriggerStart = true;
        if (const YAML::Node& triggerNode = scriptNode["ShouldTriggerStart"])
        {
            shouldTriggerStart = triggerNode.as<bool>(true);
        }

        component.scriptPaths[eastl::string(path.c_str())] = ScriptInfo{shouldTriggerStart};
    }

    return component;
}

inline bool HasPrefab(const YAML::Node& node, bool packed = true)
{
    if (!node || node.IsNull()) return false;
    if (packed) {
        return node["PrefabComponent"] && node["PrefabComponent"].IsDefined();
    } else {
        return node["Path"] && !node["Path"].IsNull();
    }
}

inline PrefabComponent GetPrefab(const YAML::Node& node, bool packed = true)
{
    PrefabComponent prefab;
    if (!node || node.IsNull()) return prefab;

    YAML::Node prefabNode;
    if (packed) {
        prefabNode = node["PrefabComponent"];
    } else {
        prefabNode = node;
    }
    if (prefabNode["Path"])
        prefab.Path = prefabNode["Path"].as<std::string>();

    return prefab;
}

inline bool HasDirectionalLight(const YAML::Node& node, bool packed = true)
{
    if (!node || node.IsNull()) return false;
    if (packed) {
        return node["DirectionalLightComponent"] && node["DirectionalLightComponent"].IsDefined();
    } else {
        return node["Color"] && node["Color"].IsDefined();
    }
}

inline DirectionalLightComponent GetDirectionalLight(const YAML::Node& node, bool packed = true)
{
    DirectionalLightComponent light;
    YAML::Node lightNode;
    if (packed) {
        lightNode = node["DirectionalLightComponent"];
    } else {
        lightNode = node;
    }
    if (!lightNode || !lightNode["Color"]) return light;

    const YAML::Node& colorNode = lightNode["Color"];
    light.Color = {
        colorNode["R"].as<float>(1.0f),
        colorNode["G"].as<float>(1.0f),
        colorNode["B"].as<float>(1.0f),
        colorNode["A"].as<float>(1.0f)
    };

    return light;
}

inline bool HasPointLight(const YAML::Node& node, bool packed = true)
{
    if (!node || node.IsNull()) return false;
    if (packed) {
        return node["PointLightComponent"] && node["PointLightComponent"].IsDefined();
    } else {
        return node["Color"] && node["Color"].IsDefined();
    }
}

inline PointLightComponent GetPointLight(const YAML::Node& node, bool packed = true)
{
    PointLightComponent light;
    YAML::Node lightNode;
    if (packed) {
        lightNode = node["PointLightComponent"];
    } else {
        lightNode = node;
    }
    if (!lightNode || !lightNode["Color"]) return light;

    const YAML::Node& colorNode = lightNode["Color"];
    light.Color = {
        colorNode["R"].as<float>(1.0f),
        colorNode["G"].as<float>(1.0f),
        colorNode["B"].as<float>(1.0f),
        colorNode["A"].as<float>(1.0f)
    };

    light.FalloffStart = lightNode["FalloffStart"].as<float>(1.0f);
    light.FalloffEnd = lightNode["FalloffEnd"].as<float>(10.0f);
    light.Intensity = lightNode["Intensity"].as<float>(1.0f);

    return light;
}

inline bool HasSpotLight(const YAML::Node& node, bool packed = true)
{
    if (!node || node.IsNull()) return false;
    if (packed) {
        return node["SpotLightComponent"] && node["SpotLightComponent"].IsDefined();
    } else {
        return node["Color"] && node["Color"].IsDefined();
    }
}

inline SpotLightComponent GetSpotLight(const YAML::Node& node, bool packed = true)
{
    SpotLightComponent light;
    YAML::Node lightNode;
    if (packed) {
        lightNode = node["SpotLightComponent"];
    } else {
        lightNode = node;
    }
    if (!lightNode || !lightNode["Color"]) return light;

    const YAML::Node& colorNode = lightNode["Color"];
    light.Color = {
        colorNode["R"].as<float>(1.0f),
        colorNode["G"].as<float>(1.0f),
        colorNode["B"].as<float>(1.0f),
        colorNode["A"].as<float>(1.0f)
    };

    light.FalloffStart = lightNode["FalloffStart"].as<float>(1.0f);
    light.FalloffEnd = lightNode["FalloffEnd"].as<float>(10.0f);
    light.Intensity = lightNode["Intensity"].as<float>(1.0f);
    light.SpotInnerAngle = lightNode["InnerAngle"].as<float>(30.0f);
    light.SpotOuterAngle = lightNode["OuterAngle"].as<float>(45.0f);

    return light;
}

inline bool HasNavMeshVolume(const YAML::Node& node, bool packed = true)
{
    if (!node || node.IsNull()) return false;
    if (packed) {
        return node["NavmeshVolumeComponent"] && node["NavmeshVolumeComponent"].IsDefined();
    } else {
        return node["Extent"] && node["Extent"].IsDefined();
    }
}

inline NavmeshVolumeComponent GetNavMeshVolume(const YAML::Node& node, bool packed = true)
{
    NavmeshVolumeComponent component;
    if (!node || node.IsNull()) return component;

    YAML::Node volumeNode;
    if (packed) {
        volumeNode = node["NavmeshVolumeComponent"];
    } else {
        volumeNode = node;
    }
    if (!volumeNode || volumeNode.IsNull()) return component;

    Vec3 extents(1.0f, 1.0f, 1.0f);
    if (const YAML::Node& extentNode = volumeNode["Extent"])
    {
        extents.x = extentNode["X"].as<float>(1.0f);
        extents.y = extentNode["Y"].as<float>(1.0f);
        extents.z = extentNode["Z"].as<float>(1.0f);
    }

    component.LocalBounds = JPH::AABox::sFromTwoPoints(
        {-extents.x, -extents.y, -extents.z},
        {extents.x, extents.y, extents.z}
    );

    component.IsEnabled = volumeNode["IsEnabled"].as<bool>(true);
    component.CellSize = volumeNode["CellSize"].as<float>(0.3f);
    component.AgentHeight = volumeNode["AgentHeight"].as<float>(2.0f);
    component.AgentRadius = volumeNode["AgentRadius"].as<float>(0.5f);
    component.AgentMaxClimb = volumeNode["AgentMaxClimb"].as<float>(0.4f);
    component.AgentMaxSlope = volumeNode["AgentMaxSlope"].as<float>(45.0f);

    return component;
}

inline bool HasStimulus(const YAML::Node& node, bool packed = true)
{
    if (!node || node.IsNull()) return false;
    if (packed) {
        return node["StimulusComponent"] && node["StimulusComponent"].IsDefined();
    } else {
        return node["EnableSight"] || node["EnableSound"] || node["EnableTouch"] || node["EnableDamage"];
    }
}

inline StimulusComponent GetStimulus(const YAML::Node& node, bool packed = true)
{
    StimulusComponent component;
    if (!node || node.IsNull() || !node.IsMap()) return component;

    YAML::Node stimulusNode;
    if (packed) {
        stimulusNode = node["StimulusComponent"];
    } else {
        stimulusNode = node;
    }
    if (!stimulusNode || stimulusNode.IsNull()) return component;

    component.enableSight = stimulusNode["EnableSight"].as<bool>(true);
    component.enableSound = stimulusNode["EnableSound"].as<bool>(true);
    component.enableTouch = stimulusNode["EnableTouch"].as<bool>(true);
    component.enableDamage = stimulusNode["EnableDamage"].as<bool>(true);

    component.sightRadius = stimulusNode["SightRadius"].as<float>(20.0f);
    component.soundRadius = stimulusNode["SoundRadius"].as<float>(30.0f);
    component.tag = stimulusNode["Tag"].as<std::string>("").c_str();
    component.enabled = stimulusNode["Enabled"].as<bool>(true);

    return component;
}

inline bool HasPerception(const YAML::Node& node, bool packed = true)
{
    if (!node || node.IsNull()) return false;
    if (packed) {
        return node["PerceptionComponent"] && node["PerceptionComponent"].IsDefined();
    } else {
        return node["EnableSight"] || node["EnableSound"] || node["EnableTouch"] || node["EnableDamage"];
    }
}

inline PerceptionComponent GetPerception(const YAML::Node& node, bool packed = true)
{
    PerceptionComponent component;
    if (!node || node.IsNull()) return component;

    YAML::Node perceptionNode;
    if (packed) {
        perceptionNode = node["PerceptionComponent"];
    } else {
        perceptionNode = node;
    }
    if (!perceptionNode || perceptionNode.IsNull()) return component;

    component.enableSight = perceptionNode["EnableSight"].as<bool>(false);
    component.sightRange = perceptionNode["SightRange"].as<float>(20.0f);
    component.sightFOV = perceptionNode["SightFOV"].as<float>(90.0f);
    component.sightForgetTime = perceptionNode["SightForgetTime"].as<float>(5.0f);
    component.sightLOSCheckInterval = perceptionNode["SightLOSCheckInterval"].as<float>(0.5f);
    component.sightRequireLOS = perceptionNode["SightRequireLOS"].as<bool>(true);

    component.enableSound = perceptionNode["EnableSound"].as<bool>(false);
    component.soundRange = perceptionNode["SoundRange"].as<float>(30.0f);
    component.soundForgetTime = perceptionNode["SoundForgetTime"].as<float>(10.0f);
    component.soundMinStrength = perceptionNode["SoundMinStrength"].as<float>(0.1f);

    component.enableTouch = perceptionNode["EnableTouch"].as<bool>(false);
    component.touchForgetTime = perceptionNode["TouchForgetTime"].as<float>(2.0f);

    component.enableDamage = perceptionNode["EnableDamage"].as<bool>(false);
    component.damageForgetTime = perceptionNode["DamageForgetTime"].as<float>(15.0f);

    component.updateInterval = perceptionNode["UpdateInterval"].as<float>(0.2f);
    component.maxUpdateDistance = perceptionNode["MaxUpdateDistance"].as<float>(50.0f);
    component.enabled = perceptionNode["Enabled"].as<bool>(true);

    if (const YAML::Node& ignoreTagsNode = perceptionNode["IgnoreTags"])
    {
        if (ignoreTagsNode.IsSequence())
        {
            component.ignoreTags.clear();
            for (const auto& tagNode : ignoreTagsNode)
            {
                std::string tagStr = tagNode.as<std::string>("");
                if (!tagStr.empty()) component.ignoreTags.push_back(tagStr.c_str());
            }
        }
    }

    if (const YAML::Node& priorityTagsNode = perceptionNode["PriorityTags"])
    {
        if (priorityTagsNode.IsSequence())
        {
            component.priorityTags.clear();
            for (const auto& tagNode : priorityTagsNode)
            {
                std::string tagStr = tagNode.as<std::string>("");
                if (!tagStr.empty()) component.priorityTags.push_back(tagStr.c_str());
            }
        }
    }

    return component;
}

inline bool HasRelationship(const YAML::Node& node, bool packed = true)
{
    if (!node || node.IsNull()) return false;
    if (packed) {
        return node["RelationshipComponent"] && node["RelationshipComponent"].IsDefined();
    } else {
        return node["Parent"] || (node["Children"] && node["Children"].IsSequence());
    }
}

inline RelationshipComponent GetRelationship(const YAML::Node& node, bool packed = true)
{
    RelationshipComponent relationship;
    if (!node || node.IsNull()) return relationship;

    YAML::Node relNode;
    if (packed) {
        relNode = node["RelationshipComponent"];
    } else {
        relNode = node;
    }
    if (!relNode || relNode.IsNull()) return relationship;

    if (relNode["Parent"])
    {
        std::string parentIdStr = relNode["Parent"].as<std::string>();
        relationship.ParentHandle = uuid::fromStrFactory(parentIdStr);
    }

    if (const YAML::Node& childrenNode = relNode["Children"])
    {
        if (childrenNode.IsSequence())
        {
            relationship.Children.clear();
            for (const auto& childNode : childrenNode)
            {
                std::string childIdStr = childNode.as<std::string>("");
                if (!childIdStr.empty())
                    relationship.Children.push_back(uuid::fromStrFactory(childIdStr));
            }
        }
    }

    return relationship;
}

inline bool HasPhysics(const YAML::Node& node, bool packed = true)
{
    if (!node || node.IsNull()) return false;
    if (packed) {
        return node["PhysicsComponent"] && node["PhysicsComponent"].IsDefined();
    } else {
        return node["ShapeType"] || node["MotionType"] || node["GravityFactor"];
    }
}

inline void GetPhysics(const YAML::Node& node, const Entity& entity, bool packed = true)
{
    if (!node || node.IsNull()) return;

    YAML::Node physNode;
    if (packed) {
        physNode = node["PhysicsComponent"];
    } else {
        physNode = node;
    }
    if (!physNode || physNode.IsNull())
    {
        BF_ERROR("Failed to parse physics component. Not found in .scene!");
        return;
    }

    ComponentShapeType shapeType = static_cast<ComponentShapeType>(physNode["ShapeType"].as<int>(0));
    PhysicsComponentMotionType motionType = static_cast<PhysicsComponentMotionType>(physNode["MotionType"].as<int>(0));
    float gravityFactor = physNode["GravityFactor"].as<float>(1.0f);
    bool isTrigger = physNode["IsTrigger"].as<bool>(false);
    ObjectLayer layer = static_cast<ObjectLayer>(physNode["ObjectLayer"].as<int>(static_cast<int>(Layers::MOVING)));

    uint8_t constraints = static_cast<uint8_t>(AllowedDOFs::All);
    if (physNode["Constraints"])
    {
        constraints = static_cast<uint8_t>(physNode["Constraints"].as<int>(static_cast<int>(AllowedDOFs::All)));
    }

    ShapeCreationSettings shapeSettings(shapeType);
    if (const YAML::Node& shapeSettingsNode = physNode["ShapeSettings"])
    {
        if (shapeSettingsNode["HalfHeight"])
            shapeSettings.halfCylinderHeight = shapeSettingsNode["HalfHeight"].as<float>(0.5f);
        if (shapeSettingsNode["Radius"])
            shapeSettings.radius = shapeSettingsNode["Radius"].as<float>(0.5f);
        if (const YAML::Node& extentsNode = shapeSettingsNode["HalfExtent"])
        {
            shapeSettings.halfExtents = Vec3(
                extentsNode["X"].as<float>(0.5f),
                extentsNode["Y"].as<float>(0.5f),
                extentsNode["Z"].as<float>(0.5f)
            );
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

inline bool HasAIController(const YAML::Node& node, bool packed = true)
{
    if (!node || node.IsNull()) return false;
    if (packed) {
        return node["AIControllerComponent"] && node["AIControllerComponent"].IsDefined();
    } else {
        return node["Path"] && !node["Path"].IsNull();
    }
}

inline void GetAIController(const YAML::Node& node, const Entity& entity, bool packed = true)
{
    if (!node || node.IsNull() || !node.IsMap()) return;

    YAML::Node aiControllerNode;
    if (packed) {
        aiControllerNode = node["AIControllerComponent"];
    } else {
        aiControllerNode = node;
    }

    if (!aiControllerNode || aiControllerNode.IsNull()) return;

    std::string path = aiControllerNode["Path"].as<std::string>("");
    float movementSpeed = aiControllerNode["MovementSpeed"].as<float>(2.0f);
    float stoppingDistance = aiControllerNode["StoppingDistance"].as<float>(0.5f);
    float groundOffset = aiControllerNode["GroundOffset"].as<float>(0.5f);
    bool faceDirection = aiControllerNode["FaceMovementDirection"].as<bool>(true);
    float rotationSpeed = aiControllerNode["RotationSpeed"].as<float>(0.5f);

    auto comp = AIControllerComponent();
    comp.scriptPath = path;
    comp.MovementSpeed = movementSpeed;
    comp.StoppingDistance = stoppingDistance;
    comp.GroundOffset = groundOffset;
    comp.FaceMovementDirection = faceDirection;
    comp.RotationSpeed = rotationSpeed;

    AISubsystem::GetInstance().CreateAttachAIControllerComponent(entity, comp);
}

inline std::string NavMeshData(const YAML::Node& node)
{
    if (!node || node.IsNull()) return "";
    if (!node["NavMeshData"] || !node["NavMeshData"]["Path"]) return "";
    return node["NavMeshData"]["Path"].as<std::string>();
}

} // namespace Blainn