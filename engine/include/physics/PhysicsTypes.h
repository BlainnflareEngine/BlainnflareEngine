#pragma once

#include <cstdint>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/MotionType.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>

namespace Blainn
{
using PhysicsComponentMotionType = JPH::EMotionType;

enum class ComponentShapeType
{
    Sphere = JPH::EShapeSubType::Sphere,
    Box = JPH::EShapeSubType::Box,
    Capsule = JPH::EShapeSubType::Capsule,
    Cylinder = JPH::EShapeSubType::Cylinder
};
} // namespace Blainn