// ComponentRegistry.h
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
    eastl::function<bool(const YAML::Node &, bool)> hasComponent;
    eastl::function<void(Entity &, const YAML::Node &, bool)> deserializer;
    eastl::function<void(Entity &, YAML::Emitter &)> serializer;
};

inline eastl::vector<eastl::pair<entt::id_type, ComponentMeta>> g_componentRegistry;

template <typename ComponentType>
void RegisterComponent(const char *name, eastl::function<bool(const YAML::Node &, bool)> hasFn,
                       eastl::function<void(Entity &, const YAML::Node &, bool)> deserializeFn,
                       eastl::function<void(Entity &, YAML::Emitter &)> serializeFn)
{
    g_componentRegistry.emplace_back(entt::type_hash<ComponentType>::value(),
                                     ComponentMeta{name, hasFn, deserializeFn, serializeFn});
}

inline void InitializeComponentRegistry()
{
    RegisterComponent<TagComponent>(
        "TagComponent", HasTag,
        [](Entity &e, const YAML::Node &node, bool packed)
        {
            if (e.HasComponent<TagComponent>()) e.GetComponent<TagComponent>().Tag = eastl::move(GetTag(node, packed));
            else e.AddComponent<TagComponent>(eastl::move(GetTag(node, packed)));
        },
        [](Entity &e, YAML::Emitter &out) { Serializer::Tag(e, out); });

    RegisterComponent<TransformComponent>(
        "TransformComponent", HasTransform,
        [](Entity &e, const YAML::Node &node, bool packed)
        {
            if (e.HasComponent<TransformComponent>())
                e.GetComponent<TransformComponent>() = eastl::move(GetTransform(node, packed));
            else e.AddComponent<TransformComponent>(eastl::move(GetTransform(node, packed)));
        },
        [](Entity &e, YAML::Emitter &out) { Serializer::Transform(e, out); });

    RegisterComponent<RelationshipComponent>(
        "RelationshipComponent", HasRelationship,
        [](Entity &e, const YAML::Node &node, bool packed)
        {
            auto comp = GetRelationship(node, packed);
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
        [](Entity &e, const YAML::Node &node, bool packed)
        {
            if (e.HasComponent<DirectionalLightComponent>())
                e.GetComponent<DirectionalLightComponent>() = eastl::move(GetDirectionalLight(node, packed));
            else e.AddComponent<DirectionalLightComponent>(eastl::move(GetDirectionalLight(node, packed)));
        },
        [](Entity &e, YAML::Emitter &out) { Serializer::DirectionalLight(e, out); });

    RegisterComponent<PointLightComponent>(
        "PointLightComponent", HasPointLight,
        [](Entity &e, const YAML::Node &node, bool packed)
        {
            if (e.HasComponent<PointLightComponent>())
                e.GetComponent<PointLightComponent>() = eastl::move(GetPointLight(node, packed));
            else e.AddComponent<PointLightComponent>(eastl::move(GetPointLight(node, packed)));
        },
        [](Entity &e, YAML::Emitter &out) { Serializer::PointLight(e, out); });

    RegisterComponent<SpotLightComponent>(
        "SpotLightComponent", HasSpotLight,
        [](Entity &e, const YAML::Node &node, bool packed)
        {
            if (e.HasComponent<SpotLightComponent>())
                e.GetComponent<SpotLightComponent>() = eastl::move(GetSpotLight(node, packed));
            else e.AddComponent<SpotLightComponent>(eastl::move(GetSpotLight(node, packed)));
        },
        [](Entity &e, YAML::Emitter &out) { Serializer::SpotLight(e, out); });

    RegisterComponent<PhysicsComponent>(
        "PhysicsComponent", HasPhysics, [](Entity &e, const YAML::Node &node, bool packed)
        { GetPhysics(node, e, packed); }, [](Entity &e, YAML::Emitter &out) { Serializer::Physics(e, out); });

    RegisterComponent<MeshComponent>(
        "MeshComponent", HasMesh,
        [](Entity &e, const YAML::Node &node, bool packed)
        {
            if (e.HasComponent<MeshComponent>()) e.GetComponent<MeshComponent>() = eastl::move(GetMesh(node, packed));
            else e.AddComponent<MeshComponent>(eastl::move(GetMesh(node, packed)));
        },
        [](Entity &e, YAML::Emitter &out) { Serializer::Mesh(e, out); });

    RegisterComponent<CameraComponent>(
        "CameraComponent", HasCamera,
        [](Entity &e, const YAML::Node &node, bool packed)
        {
            if (e.HasComponent<CameraComponent>())
                e.GetComponent<CameraComponent>() = eastl::move(GetCamera(node, packed));
            else e.AddComponent<CameraComponent>(eastl::move(GetCamera(node, packed)));
        },
        [](Entity &e, YAML::Emitter &out) { Serializer::Camera(e, out); });

    RegisterComponent<SkyboxComponent>(
        "SkyboxComponent", HasSkybox,
        [](Entity &e, const YAML::Node &node, bool packed)
        {
            if (e.HasComponent<SkyboxComponent>())
                e.GetComponent<SkyboxComponent>() = eastl::move(GetSkybox(node, packed));
            else e.AddComponent<SkyboxComponent>(eastl::move(GetSkybox(node, packed)));
        },
        [](Entity &e, YAML::Emitter &out) { Serializer::Skybox(e, out); });

    RegisterComponent<NavmeshVolumeComponent>(
        "NavmeshVolumeComponent", HasNavMeshVolume,
        [](Entity &e, const YAML::Node &node, bool packed)
        {
            if (e.HasComponent<NavmeshVolumeComponent>())
                e.GetComponent<NavmeshVolumeComponent>() = eastl::move(GetNavMeshVolume(node, packed));
            else e.AddComponent<NavmeshVolumeComponent>(eastl::move(GetNavMeshVolume(node, packed)));
        },
        [](Entity &e, YAML::Emitter &out) { Serializer::NavMeshVolume(e, out); });

    RegisterComponent<ScriptingComponent>(
        "ScriptingComponent", HasScripting,
        [](Entity &e, const YAML::Node &node, bool packed)
        {
            if (e.HasComponent<ScriptingComponent>())
                e.GetComponent<ScriptingComponent>() = eastl::move(GetScripting(node, packed));
            else e.AddComponent<ScriptingComponent>(eastl::move(GetScripting(node, packed)));
        },
        [](Entity &e, YAML::Emitter &out) { Serializer::Scripting(e, out); });

    RegisterComponent<AIControllerComponent>(
        "AIControllerComponent", HasAIController, [](Entity &e, const YAML::Node &node, bool packed)
        { GetAIController(node, e, packed); }, [](Entity &e, YAML::Emitter &out) { Serializer::AIController(e, out); });

    RegisterComponent<StimulusComponent>(
        "StimulusComponent", HasStimulus,
        [](Entity &e, const YAML::Node &node, bool packed)
        {
            if (e.HasComponent<StimulusComponent>())
                e.GetComponent<StimulusComponent>() = eastl::move(GetStimulus(node, packed));
            else e.AddComponent<StimulusComponent>(eastl::move(GetStimulus(node, packed)));
        },
        [](Entity &e, YAML::Emitter &out) { Serializer::Stimulus(e, out); });

    RegisterComponent<PerceptionComponent>(
        "PerceptionComponent", HasPerception,
        [](Entity &e, const YAML::Node &node, bool packed)
        {
            if (e.HasComponent<PerceptionComponent>())
                e.GetComponent<PerceptionComponent>() = eastl::move(GetPerception(node, packed));
            else e.AddComponent<PerceptionComponent>(eastl::move(GetPerception(node, packed)));
        },
        [](Entity &e, YAML::Emitter &out) { Serializer::Perception(e, out); });

    RegisterComponent<PrefabComponent>(
        "PrefabComponent", HasPrefab,
        [](Entity &e, const YAML::Node &node, bool packed)
        {
            if (e.HasComponent<PrefabComponent>()) return;
            e.AddComponent<PrefabComponent>(eastl::move(GetPrefab(node, packed)));
        },
        [](Entity &e, YAML::Emitter &out) { Serializer::Prefab(e, out); });
}

inline const ComponentMeta *FindComponentMeta(const entt::id_type typeId)
{
    for (const auto &[id, meta] : g_componentRegistry)
    {
        if (id == typeId) return &meta;
    }
    return nullptr;
}

} // namespace Blainn