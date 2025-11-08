#pragma once

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Body/BodyID.h"

#include "aliases.h"
#include "physics/PhysicsTypes.h"
#include "physics/ShapeFactory.h"

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