//
// Created by gorev on 05.02.2026.
//

#include "subsystems/PrefabSubsystem.h"

#include "ComponentRegistry.h"
#include "Engine.h"
#include "components/PrefabComponent.h"

namespace Blainn
{
void PrefabSubsystem::UpdatePrefabRefs(const Path &relativePath)
{
    // TODO: Get prefab from relative path
    Path absolutePath = Engine::GetContentDirectory() / relativePath;
    YAML::Node prefab = YAML::LoadFile(absolutePath.string());

    for (auto &scene : Engine::GetSceneManager().GetActiveScenes())
    {
        const auto &view = scene->GetAllEntitiesWith<IDComponent, PrefabComponent>();
        for (const auto &[entity, id, prefabComponent] : view.each())
        {
            if (prefabComponent.Path == relativePath)
            {
                // TODO: change all components
            }
        }
    }
}


void PrefabSubsystem::RegisterComponentOverride(Entity entity, entt::id_type componentType)
{
    if (!entity.HasComponent<PrefabComponent>()) return;

    auto &prefabComp = entity.GetComponent<PrefabComponent>();

    if (prefabComp.Overrides.find(componentType) != prefabComp.Overrides.end()) return;

    auto prefabYaml = LoadPrefabYaml(prefabComp.Path);
    if (!prefabYaml) return;

    auto entitiesNode = prefabYaml["Entities"];
    if (!entitiesNode.IsSequence() || entitiesNode.size() == 0) return;

    const auto &rootEntityNode = entitiesNode[0];

    const auto *meta = FindComponentMeta(componentType);
    if (!meta) return;

    if (!meta->hasComponent(rootEntityNode, true)) return;

    YAML::Node componentNode = rootEntityNode[meta->name.c_str()];

    if (!componentNode) return;

    prefabComp.Overrides[componentType] = PrefabOverride{componentType, componentNode};

    BF_DEBUG("Registered override for component {} in prefab {}", meta->name, prefabComp.Path.string());
}


YAML::Node PrefabSubsystem::LoadPrefabYaml(const Path &relativePath)
{
    Path absolutePath = Engine::GetContentDirectory() / relativePath;
    return YAML::LoadFile(absolutePath.string());
}


bool PrefabSubsystem::HasPrefabOverrides(Entity entity)
{
    auto component = entity.TryGetComponent<PrefabComponent>();
    if (!component) return false;

    return component->Overrides.empty();
}


void PrefabSubsystem::ApplyPrefabOverrides(Entity entity)
{
    // TODO: make prefab override & notify all prefabs
}


void PrefabSubsystem::RevertPrefabOverrides(Entity entity)
{
    auto &prefabComp = entity.GetComponent<PrefabComponent>();

    for (auto &[componentType, overrideData] : prefabComp.Overrides)
    {
        const auto *meta = FindComponentMeta(componentType);
        if (!meta) continue;

        meta->deserializer(entity, overrideData.OriginalData, false);
        BF_DEBUG("Reverted component {} to prefab state", meta->name);
    }

    prefabComp.Overrides.clear();
    BF_DEBUG("All prefab overrides reverted for entity");
}


eastl::any PrefabSubsystem::GetOriginalPrefabValue(Entity entity, entt::id_type componentType,
                                                   const eastl::string &fieldPath)
{
    return eastl::any();
}
} // namespace Blainn