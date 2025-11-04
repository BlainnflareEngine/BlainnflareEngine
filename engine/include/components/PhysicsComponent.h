#pragma once

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Body/BodyID.h"

#include "aliases.h"

namespace Blainn
{
struct PhysicsComponent
{
    uuid m_parentId = {};
    JPH::BodyID m_bodyId = JPH::BodyID();
};
} // namespace Blainn