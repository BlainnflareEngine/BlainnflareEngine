#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>

#include "aliases.h"
#include "physics/PhysicsTypes.h"
#include "physics/ShapeFactory.h"

namespace eastl
{
template <> struct hash<JPH::BodyID>
{
    size_t operator()(const JPH::BodyID &bodyId) const EA_NOEXCEPT
    {
        return std::hash<JPH::BodyID>{}(bodyId); // must return size_t
    }
};
} // namespace eastl

namespace Blainn
{
struct PhysicsComponent
{
    uuid parentId = {};
    JPH::BodyID bodyId = JPH::BodyID();
    ComponentShapeType shapeType = ComponentShapeType::Empty;
    ShapeHierarchy shapeHierarchy = {};
    // TODO: simulation enabled, gravity, other settings?
};
} // namespace Blainn