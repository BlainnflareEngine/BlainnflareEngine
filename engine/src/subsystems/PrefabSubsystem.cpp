//
// Created by gorev on 05.02.2026.
//

#include "subsystems/PrefabSubsystem.h"

#include "components/PrefabComponent.h"
#include "Engine.h"

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


Entity PrefabSubsystem::CreateEntityFromPrefab(const Path &relativePath, const Scene &scene, const Vec3 &position,
                                               const Quat &rotation, const Vec3 &scale)
{
    (void)relativePath;
    (void)scene;
    (void)position;
    (void)rotation;
    (void)scale;
    assert(false && "Not implemented");
    return {};
}
} // namespace Blainn
