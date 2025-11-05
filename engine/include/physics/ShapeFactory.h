#pragma once

#include <EASTL/utility.h>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>

#include "aliases.h"

namespace Blainn
{
class ShapeFactory
{
public:
    // TODO: uuid probably not needed
    static eastl::pair<uuid, eastl::shared_ptr<JPH::Shape>> CreateSphereShape(float radius);
    static eastl::pair<uuid, eastl::shared_ptr<JPH::Shape>> CreateBoxShape(Vec3 halfExtents);
    static eastl::pair<uuid, eastl::shared_ptr<JPH::Shape>> CreateCapsuleShape(float halfHeight, float radius);
    static eastl::pair<uuid, eastl::shared_ptr<JPH::Shape>> CreateCylinderShape(float halfHeight, float radius);

private:
    ShapeFactory() = delete;
};
} // namespace Blainn