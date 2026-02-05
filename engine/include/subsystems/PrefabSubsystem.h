//
// Created by gorev on 05.02.2026.
//

#pragma once
#include "scene/Entity.h"

namespace Blainn
{
class Scene;
}
namespace Blainn
{
class PrefabSubsystem
{
public:
    void UpdatePrefabRefs(const Path &relativePath);

    Entity CreateEntityFromPrefab(const Path &relativePath, const Scene &scene, const Vec3 &position,
                                  const Quat &rotation, const Vec3 &scale);

private:
};
} // namespace Blainn