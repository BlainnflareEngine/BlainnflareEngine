#pragma once
#include <cstdint>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/MotionType.h>
#include <Jolt/Physics/Body/AllowedDOFs.h>
#include <Jolt/Physics/EActivation.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Geometry/AABox.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>

namespace Blainn
{
using PhysicsComponentMotionType = JPH::EMotionType;
using EActivation = JPH::EActivation;
using ObjectLayer = JPH::ObjectLayer;
using AABox = JPH::AABox;
using AllowedDOFs = JPH::EAllowedDOFs;

enum class ComponentShapeType : uint8_t
{
    Sphere      = static_cast<uint8_t>(JPH::EShapeSubType::Sphere),
    Box         = static_cast<uint8_t>(JPH::EShapeSubType::Box),
    Capsule     = static_cast<uint8_t>(JPH::EShapeSubType::Capsule),
    Cylinder    = static_cast<uint8_t>(JPH::EShapeSubType::Cylinder),
    Empty       = static_cast<uint8_t>(JPH::EShapeSubType::Empty)
};
} // namespace Blainn
