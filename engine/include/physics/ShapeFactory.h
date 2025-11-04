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
    // TODO: static methods to create Jolt shapes
    static eastl::pair<uuid, eastl::shared_ptr<JPH::Shape>> CreateSphereShape(float radius);

private:
    ShapeFactory() = delete;
};
} // namespace Blainn