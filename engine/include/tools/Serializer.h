//
// Created by gorev on 10.11.2025.
//

#pragma once
#include "AssetManager.h"
#include "Log.h"
#include "PhysicsSubsystem.h"
#include "components/AIControllerComponent.h"
#include "components/CameraComponent.h"
#include "components/LightComponent.h"
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
#include "Engine.h"

namespace Blainn
{
class Serializer
{
public:
    static void CreatePrefab(Entity &entity, const Path &relativePath);

    static void Name(Entity &entity, YAML::Emitter &out);

    static void ID(Entity &entity, YAML::Emitter &out);

    static void Tag(Entity &entity, YAML::Emitter &out);

    static void Transform(Entity &entity, YAML::Emitter &out);

    static void Relationship(Entity &entity, YAML::Emitter &out);

    static void Scripting(Entity &entity, YAML::Emitter &out);

    static void AIController(Entity &entity, YAML::Emitter &out);

    static void Mesh(Entity &entity, YAML::Emitter &out);

    static void Physics(Entity &entity, YAML::Emitter &out);

    static void Camera(Entity &entity, YAML::Emitter &out);

    static void Skybox(Entity &entity, YAML::Emitter &out);

    static void NavMeshVolume(Entity &entity, YAML::Emitter &out);

    static void ExistingNavMeshData(const Path &absolutePath, YAML::Emitter &out);

    static void Perception(Entity &entity, YAML::Emitter &out);

    static void Stimulus(Entity &entity, YAML::Emitter &out);

    static void DirectionalLight(Entity &entity, YAML::Emitter &out);

    static void PointLight(Entity &entity, YAML::Emitter &out);

    static void SpotLight(Entity &entity, YAML::Emitter &out);
};
} // namespace Blainn
