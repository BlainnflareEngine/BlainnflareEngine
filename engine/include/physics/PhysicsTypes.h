#pragma once

namespace Blainn
{
using PhysicsComponentMotionType = JPH::EMotionType;
using EActivation = JPH::EActivation;
using ObjectLayer = JPH::ObjectLayer;
using AABox = JPH::AABox;
using AllowedDOFs = JPH::EAllowedDOFs;

enum class ComponentShapeType
{
    Sphere = JPH::EShapeSubType::Sphere,
    Box = JPH::EShapeSubType::Box,
    Capsule = JPH::EShapeSubType::Capsule,
    Cylinder = JPH::EShapeSubType::Cylinder,
    Empty = JPH::EShapeSubType::Empty
};
} // namespace Blainn