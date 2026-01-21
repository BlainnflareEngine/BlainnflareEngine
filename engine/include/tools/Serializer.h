//
// Created by gorev on 10.11.2025.
//

#pragma once
#include "AssetManager.h"
#include "Log.h"
#include "PhysicsSubsystem.h"
#include "components/AIControllerComponent.h"
#include "components/CameraComponent.h"
#include "components/MeshComponent.h"
#include "components/NavMeshVolumeComponent.h"
#include "components/PhysicsComponent.h"
#include "components/ScriptingComponent.h"
#include "components/SkyboxComponent.h"
#include "components/PerceptionComponent.h"
#include "components/StimulusComponent.h"
#include "physics/BodyGetter.h"
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

        Vec3 translation = transform.GetTranslation();
        out << YAML::Key << "Translation" << YAML::Value << YAML::BeginMap;
        out << YAML::Key << "x" << YAML::Value << translation.x;
        out << YAML::Key << "y" << YAML::Value << translation.y;
        out << YAML::Key << "z" << YAML::Value << translation.z;
        out << YAML::EndMap;

        out << YAML::Key << "Rotation" << YAML::Value << YAML::BeginMap;
        out << YAML::Key << "x" << YAML::Value << transform.GetRotationEuler().x;
        out << YAML::Key << "y" << YAML::Value << transform.GetRotationEuler().y;
        out << YAML::Key << "z" << YAML::Value << transform.GetRotationEuler().z;
        out << YAML::EndMap;

        Vec3 scale = transform.GetScale();
        out << YAML::Key << "Scale" << YAML::Value << YAML::BeginMap;
        out << YAML::Key << "x" << YAML::Value << scale.x;
        out << YAML::Key << "y" << YAML::Value << scale.y;
        out << YAML::Key << "z" << YAML::Value << scale.z;
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

    static void AIController(Entity &entity, YAML::Emitter &out)
    {
        if (!entity.HasComponent<AIControllerComponent>()) return;

        auto &aiController = entity.GetComponent<AIControllerComponent>();

        out << YAML::Key << "AIControllerComponent" << YAML::Value << YAML::BeginMap;
        out << YAML::Key << "Path" << aiController.scriptPath.c_str();
        out << YAML::Key << "MovementSpeed" << aiController.MovementSpeed;
        out << YAML::Key << "StoppingDistance" << aiController.StoppingDistance;
        out << YAML::EndMap;
    }

    static void Mesh(Entity &entity, YAML::Emitter &out)
    {
        if (!entity.HasComponent<MeshComponent>()) return;

        auto &mesh = entity.GetComponent<MeshComponent>();

        out << YAML::Key << "MeshComponent" << YAML::Value << YAML::BeginMap;
        out << YAML::Key << "Enabled" << YAML::Value << mesh.Enabled;
        out << YAML::Key << "IsWalkable" << YAML::Value << mesh.IsWalkable;
        out << YAML::Key << "Path" << YAML::Value << AssetManager::GetInstance().GetMeshPath(*mesh.MeshHandle).string();
        out << YAML::Key << "Material" << YAML::Value
            << AssetManager::GetInstance().GetMaterialPath(*mesh.MaterialHandle).string();
        out << YAML::EndMap;
    }

    static void Physics(Entity &entity, YAML::Emitter &out)
    {
        if (!entity.HasComponent<PhysicsComponent>()) return;

        auto &physics = entity.GetComponent<PhysicsComponent>();

        out << YAML::Key << "PhysicsComponent" << YAML::Value << YAML::BeginMap;
        out << YAML::Key << "ParentID" << YAML::Value << physics.parentId.str();
        out << YAML::Key << "ShapeType" << YAML::Value << static_cast<int>(physics.GetShapeType());
        out << YAML::Key << "ControlParentTransform" << YAML::Value << physics.controlParentTransform;

        BodyGetter body = PhysicsSubsystem::GetBodyGetter(entity);
        PhysicsComponentMotionType motionType = body.GetMotionType();
        float gravityFactor = motionType == PhysicsComponentMotionType::Static ? 0.0f : body.GetGravityFactor();
        out << YAML::Key << "ObjectLayer" << YAML::Value << body.GetObjectLayer();
        out << YAML::Key << "MotionType" << YAML::Value << static_cast<int>(motionType);
        out << YAML::Key << "GravityFactor" << YAML::Value << gravityFactor;
        out << YAML::Key << "IsTrigger" << YAML::Value << body.isTrigger();

        Transform(entity, out);

        out << YAML::Key << "ShapeSettings" << YAML::Value << YAML::BeginMap;
        switch (body.GetShapeType())
        {
        case ComponentShapeType::Box:
        {
            Vec3 vec = body.GetBoxShapeHalfExtents().value();
            out << YAML::Key << "HalfExtent" << YAML::Value << YAML::BeginMap;
            out << YAML::Key << "X" << YAML::Value << vec.x;
            out << YAML::Key << "Y" << YAML::Value << vec.y;
            out << YAML::Key << "Z" << YAML::Value << vec.z;
            out << YAML::EndMap;
            break;
        }

        case ComponentShapeType::Sphere:
        {
            float radius = body.GetSphereShapeRadius().value();
            out << YAML::Key << "Radius" << YAML::Value << radius;
            break;
        }

        case ComponentShapeType::Capsule:
        {
            auto halfHeightAndRadius = body.GetCapsuleShapeHalfHeightAndRadius().value();
            out << YAML::Key << "HalfHeight" << YAML::Value << halfHeightAndRadius.first;
            out << YAML::Key << "Radius" << YAML::Value << halfHeightAndRadius.second;
            break;
        }

        case ComponentShapeType::Cylinder:
        {
            auto halfHeightAndRadius = body.GetCylinderShapeHalfHeightAndRadius().value();
            out << YAML::Key << "HalfHeight" << YAML::Value << halfHeightAndRadius.first;
            out << YAML::Key << "Radius" << YAML::Value << halfHeightAndRadius.second;
            break;
        }

        case ComponentShapeType::Empty:
            break;
        }

        out << YAML::EndMap;
        out << YAML::EndMap;
    }

    static void Camera(Entity &entity, YAML::Emitter &out)
    {
        if (!entity.HasComponent<CameraComponent>()) return;

        auto camera = entity.GetComponent<CameraComponent>();
        out << YAML::Key << "CameraComponent" << YAML::Value << YAML::BeginMap;
        out << YAML::Key << "Priority" << YAML::Value << camera.CameraPriority;
        out << YAML::Key << "CameraSettings" << YAML::Value << YAML::BeginMap;
        out << YAML::Key << "FOV" << YAML::Value << camera.camera.GetFovDegrees();
        out << YAML::Key << "NearZ" << YAML::Value << camera.camera.GetNearZ();
        out << YAML::Key << "FarZ" << YAML::Value << camera.camera.GetFarZ();

        out << YAML::EndMap;
        out << YAML::EndMap;
    }

    static void Skybox(Entity &entity, YAML::Emitter &out)
    {
        if (!entity.HasComponent<SkyboxComponent>()) return;

        auto skybox = entity.GetComponent<SkyboxComponent>();
        out << YAML::Key << "SkyboxComponent" << YAML::Value << YAML::BeginMap;
        out << YAML::Key << "Path" << YAML::Value
            << AssetManager::GetInstance().GetTexturePath(*skybox.textureHandle).string();
        out << YAML::EndMap;
    }

    static void NavMeshVolume(Entity &entity, YAML::Emitter &out)
    {
        if (!entity.HasComponent<NavmeshVolumeComponent>()) return;

        auto volume = entity.GetComponent<NavmeshVolumeComponent>();
        out << YAML::Key << "NavmeshVolumeComponent" << YAML::Value << YAML::BeginMap;

        out << YAML::Key << "Extent" << YAML::Value << YAML::BeginMap;
        out << YAML::Key << "X" << volume.LocalBounds.GetExtent().GetX();
        out << YAML::Key << "Y" << volume.LocalBounds.GetExtent().GetY();
        out << YAML::Key << "Z" << volume.LocalBounds.GetExtent().GetZ();
        out << YAML::EndMap;

        out << YAML::Key << "Enabled" << YAML::Value << volume.IsEnabled;
        out << YAML::Key << "CellSize" << YAML::Value << volume.CellSize;
        out << YAML::Key << "AgentHeight" << YAML::Value << volume.AgentHeight;
        out << YAML::Key << "AgentRadius" << YAML::Value << volume.AgentRadius;
        out << YAML::Key << "AgentMaxClimb" << YAML::Value << volume.AgentMaxClimb;
        out << YAML::Key << "AgentMaxSlope" << YAML::Value << volume.AgentMaxSlope;
        out << YAML::EndMap;
    }

    static void ExistingNavMeshData(const Path &absolutePath, YAML::Emitter &out)
    {
        if (std::filesystem::exists(absolutePath))
        {
            auto node = YAML::LoadFile(absolutePath.string());

            if (node && node["NavMeshData"])
            {
                const auto &navmeshDataPath = node["NavMeshData"]["Path"].as<std::string>();
                out << YAML::Key << "NavMeshData" << YAML::Value << YAML::BeginMap;
                out << YAML::Key << "Path" << YAML::Value << navmeshDataPath;
                out << YAML::EndMap;
            }
        }
    }

    static void Perception(Entity &entity, YAML::Emitter &out)
    {
        if (!entity.HasComponent<PerceptionComponent>()) return;

        auto &perception = entity.GetComponent<PerceptionComponent>();
        out << YAML::Key << "PerceptionComponent" << YAML::Value << YAML::BeginMap;

        out << YAML::Key << "EnableSight" << YAML::Value << perception.enableSight;
        out << YAML::Key << "SightRange" << YAML::Value << perception.sightRange;
        out << YAML::Key << "SightFOV" << YAML::Value << perception.sightFOV;
        out << YAML::Key << "SightForgetTime" << YAML::Value << perception.sightForgetTime;
        out << YAML::Key << "SightLOSCheckInterval" << YAML::Value << perception.sightLOSCheckInterval;
        out << YAML::Key << "SightRequireLOS" << YAML::Value << perception.sightRequireLOS;

        out << YAML::Key << "EnableSound" << YAML::Value << perception.enableSound;
        out << YAML::Key << "SoundRange" << YAML::Value << perception.soundRange;
        out << YAML::Key << "SoundForgetTime" << YAML::Value << perception.soundForgetTime;
        out << YAML::Key << "SoundMinStrength" << YAML::Value << perception.soundMinStrength;

        out << YAML::Key << "EnableTouch" << YAML::Value << perception.enableTouch;
        out << YAML::Key << "TouchForgetTime" << YAML::Value << perception.touchForgetTime;

        out << YAML::Key << "EnableDamage" << YAML::Value << perception.enableDamage;
        out << YAML::Key << "DamageForgetTime" << YAML::Value << perception.damageForgetTime;

        out << YAML::Key << "UpdateInterval" << YAML::Value << perception.updateInterval;
        out << YAML::Key << "MaxUpdateDistance" << YAML::Value << perception.maxUpdateDistance;

        out << YAML::Key << "IgnoreTags" << YAML::Value << YAML::BeginSeq;
        for (const auto &tag : perception.ignoreTags)
            out << tag.c_str();
        out << YAML::EndSeq;

        out << YAML::Key << "PriorityTags" << YAML::Value << YAML::BeginSeq;
        for (const auto &tag : perception.priorityTags)
            out << tag.c_str();
        out << YAML::EndSeq;

        out << YAML::Key << "Enabled" << YAML::Value << perception.enabled;

        out << YAML::EndMap;
    }

    static void Stimulus(Entity &entity, YAML::Emitter &out)
    {
        if (!entity.HasComponent<StimulusComponent>()) return;

        auto &stimulus = entity.GetComponent<StimulusComponent>();
        out << YAML::Key << "StimulusComponent" << YAML::Value << YAML::BeginMap;

        out << YAML::Key << "EnableSight" << YAML::Value << stimulus.enableSight;
        out << YAML::Key << "EnableSound" << YAML::Value << stimulus.enableSound;
        out << YAML::Key << "EnableTouch" << YAML::Value << stimulus.enableTouch;
        out << YAML::Key << "EnableDamage" << YAML::Value << stimulus.enableDamage;

        out << YAML::Key << "SightRadius" << YAML::Value << stimulus.sightRadius;
        out << YAML::Key << "SoundRadius" << YAML::Value << stimulus.soundRadius;

        out << YAML::Key << "Tag" << YAML::Value << stimulus.tag.c_str();
        out << YAML::Key << "Enabled" << YAML::Value << stimulus.enabled;

        out << YAML::EndMap;
    }
};
} // namespace Blainn
