//
// Created by gorev on 26.01.2026.
//

#pragma once
#include "Serializer.h"
#include "EASTL/vector.h"
#include "EASTL/internal/function.h"
#include "scene/Entity.h"
#include "scene/SceneParser.h"
#include "yaml-cpp/emitter.h"

namespace Blainn
{
struct ComponentMeta
{
    eastl::string name;
    eastl::function<bool(const YAML::Node &)> hasComponent;
    eastl::function<void(Entity &, const YAML::Node &)> deserializer;
    eastl::function<void(Entity &, YAML::Emitter &)> serializer;
};

inline eastl::vector<eastl::pair<entt::id_type, ComponentMeta>> g_componentRegistry;

template <typename ComponentType>
void RegisterComponent(const char *name, eastl::function<bool(const YAML::Node &)> hasFn,
                       eastl::function<void(Entity &, const YAML::Node &)> deserializeFn,
                       eastl::function<void(Entity &, YAML::Emitter &)> serializeFn)
{
    g_componentRegistry.emplace_back(entt::type_hash<ComponentType>::value(),
                                     ComponentMeta{name, hasFn, deserializeFn, serializeFn});
}

inline void InitializeComponentRegistry()
{
    RegisterComponent<TransformComponent>(
        "TransformComponent", HasTransform, [](Entity &e, const YAML::Node &node)
        { e.AddComponent<TransformComponent>(eastl::move(GetTransform(node["TransformComponent"]))); },
        [](Entity &e, YAML::Emitter &out) { Serializer::Transform(e, out); });

    RegisterComponent<RelationshipComponent>(
        "RelationshipComponent", HasRelationship,
        [](Entity &e, const YAML::Node &node)
        {
            auto comp = GetRelationship(node["RelationshipComponent"]);
            if (auto existing = e.TryGetComponent<RelationshipComponent>())
            {
                e.SetParentUUID(comp.ParentHandle);
                existing->Children = eastl::move(comp.Children);
            }
            else
            {
                e.AddComponent<RelationshipComponent>(eastl::move(comp));
            }
        },
        [](Entity &e, YAML::Emitter &out) { Serializer::Relationship(e, out); });

    RegisterComponent<DirectionalLightComponent>(
        "DirectionalLightComponent", HasDirectionalLight,
        [](Entity &e, const YAML::Node &node)
        {
            e.AddComponent<DirectionalLightComponent>(
                eastl::move(GetDirectionalLight(node["DirectionalLightComponent"])));
        },
        [](Entity &e, YAML::Emitter &out) { Serializer::DirectionalLight(e, out); });

    RegisterComponent<PointLightComponent>(
        "PointLightComponent", HasPointLight, [](Entity &e, const YAML::Node &node)
        { e.AddComponent<PointLightComponent>(eastl::move(GetPointLight(node["PointLightComponent"]))); },
        [](Entity &e, YAML::Emitter &out) { Serializer::PointLight(e, out); });

    RegisterComponent<SpotLightComponent>(
        "SpotLightComponent", HasSpotLight, [](Entity &e, const YAML::Node &node)
        { e.AddComponent<SpotLightComponent>(eastl::move(GetSpotLight(node["SpotLightComponent"]))); },
        [](Entity &e, YAML::Emitter &out) { Serializer::SpotLight(e, out); });

    RegisterComponent<PhysicsComponent>(
        "PhysicsComponent", HasPhysics,
        [](Entity &e, const YAML::Node &node) { GetPhysics(node["PhysicsComponent"], e); },
        [](Entity &e, YAML::Emitter &out) { Serializer::Physics(e, out); });

    RegisterComponent<MeshComponent>(
        "MeshComponent", HasMesh, [](Entity &e, const YAML::Node &node)
        { e.AddComponent<MeshComponent>(eastl::move(GetMesh(node["MeshComponent"]))); },
        [](Entity &e, YAML::Emitter &out) { Serializer::Mesh(e, out); });

    RegisterComponent<CameraComponent>(
        "CameraComponent", HasCamera, [](Entity &e, const YAML::Node &node)
        { e.AddComponent<CameraComponent>(eastl::move(GetCamera(node["CameraComponent"]))); },
        [](Entity &e, YAML::Emitter &out) { Serializer::Camera(e, out); });

    RegisterComponent<SkyboxComponent>(
        "SkyboxComponent", HasSkybox, [](Entity &e, const YAML::Node &node)
        { e.AddComponent<SkyboxComponent>(eastl::move(GetSkybox(node["SkyboxComponent"]))); },
        [](Entity &e, YAML::Emitter &out) { Serializer::Skybox(e, out); });

    RegisterComponent<NavmeshVolumeComponent>(
        "NavmeshVolumeComponent", HasNavMeshVolume, [](Entity &e, const YAML::Node &node)
        { e.AddComponent<NavmeshVolumeComponent>(eastl::move(GetNavMeshVolume(node["NavmeshVolumeComponent"]))); },
        [](Entity &e, YAML::Emitter &out) { Serializer::NavMeshVolume(e, out); });

    RegisterComponent<ScriptingComponent>(
        "ScriptingComponent", HasScripting, [](Entity &e, const YAML::Node &node)
        { e.AddComponent<ScriptingComponent>(eastl::move(GetScripting(node["ScriptingComponent"]))); },
        [](Entity &e, YAML::Emitter &out) { Serializer::Scripting(e, out); });

    RegisterComponent<AIControllerComponent>(
        "AIControllerComponent", HasAIController,
        [](Entity &e, const YAML::Node &node) { GetAIController(node["AIControllerComponent"], e); },
        [](Entity &e, YAML::Emitter &out) { Serializer::AIController(e, out); });

    RegisterComponent<StimulusComponent>(
        "StimulusComponent", HasStimulus, [](Entity &e, const YAML::Node &node)
        { e.AddComponent<StimulusComponent>(eastl::move(GetStimulus(node["StimulusComponent"]))); },
        [](Entity &e, YAML::Emitter &out) { Serializer::Stimulus(e, out); });

    RegisterComponent<PerceptionComponent>(
        "PerceptionComponent", HasPerception, [](Entity &e, const YAML::Node &node)
        { e.AddComponent<PerceptionComponent>(eastl::move(GetPerception(node["PerceptionComponent"]))); },
        [](Entity &e, YAML::Emitter &out) { Serializer::Perception(e, out); });
}
} // namespace Blainn
