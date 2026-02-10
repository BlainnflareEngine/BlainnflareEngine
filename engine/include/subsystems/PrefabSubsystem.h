//
// Created by gorev on 05.02.2026.
//

#pragma once
#include "scene/Entity.h"

namespace eastl
{
class any;
}
namespace Blainn
{
class Scene;
}
namespace Blainn
{
class PrefabSubsystem
{
public:
    static void UpdatePrefabRefs(const Path &relativePath);

    // Register full component (for now, will be changed to register only one field)
    static void RegisterComponentOverride(Entity entity, entt::id_type componentType);
    static YAML::Node LoadPrefabYaml(const Path &relativePath);
    static bool HasPrefabOverrides(Entity entity);
    static void ApplyPrefabOverrides(Entity entity);
    static void RevertPrefabOverrides(Entity entity);
    static eastl::any GetOriginalPrefabValue(Entity entity, entt::id_type componentType,
                                             const eastl::string &fieldPath);
};
} // namespace Blainn