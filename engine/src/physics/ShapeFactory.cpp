#include "pch.h"

#include "physics/ShapeFactory.h"

#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>

#include "physics/Conversion.h"
#include "tools/random.h"

using namespace Blainn;

ShapeHierarchy ShapeFactory::CreateSphereShape(float radius)
{
    return CreateShape<JPH::SphereShape>(radius);
}

ShapeHierarchy ShapeFactory::CreateBoxShape(Vec3 halfExtents)
{
    return CreateShape<JPH::BoxShape>(ToJoltVec3(halfExtents));
}

ShapeHierarchy ShapeFactory::CreateCapsuleShape(float halfHeight, float radius)
{
    return CreateShape<JPH::CapsuleShape>(halfHeight, radius);
}

ShapeHierarchy ShapeFactory::CreateCylinderShape(float halfHeight, float radius)
{
    return CreateShape<JPH::CylinderShape>(halfHeight, radius);
}
