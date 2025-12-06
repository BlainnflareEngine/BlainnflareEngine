#pragma once

#include <cstdint>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/MotionType.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/EActivation.h>

namespace Blainn
{
using PhysicsComponentMotionType = JPH::EMotionType;
using EActivation = JPH::EActivation;
using ObjectLayer = JPH::ObjectLayer;

enum class ComponentShapeType
{
    Sphere = JPH::EShapeSubType::Sphere,
    Box = JPH::EShapeSubType::Box,
    Capsule = JPH::EShapeSubType::Capsule,
    Cylinder = JPH::EShapeSubType::Cylinder,
    Empty = JPH::EShapeSubType::Empty
};
} // namespace Blainn