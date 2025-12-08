#include "pch.h"

#include "physics/ShapeFactory.h"

#include "physics/Conversion.h"

using namespace Blainn;

ShapeHierarchy ShapeFactory::CreateSphereShape(float radius)

{
    return CreateShapeInternal<JPH::SphereShape>(radius);
}

eastl::optional<ShapeHierarchy> ShapeFactory::CreateShape(ShapeCreationSettings &settings)
{
    switch (settings.shapeType)
    {
    case ComponentShapeType::Sphere:
        return ShapeFactory::CreateSphereShape(settings.radius);
    case ComponentShapeType::Box:
        return ShapeFactory::CreateBoxShape(settings.halfExtents);
    case ComponentShapeType::Capsule:
        return ShapeFactory::CreateCapsuleShape(settings.halfCylinderHeight, settings.radius);
    case ComponentShapeType::Cylinder:
        return ShapeFactory::CreateCylinderShape(settings.halfCylinderHeight, settings.radius);
    default:
        BF_ERROR("Invalid physics shape type");
        return eastl::optional<ShapeHierarchy>{};
    }
}

ShapeHierarchy ShapeFactory::CreateBoxShape(Vec3 halfExtents)
{
    return CreateShapeInternal<JPH::BoxShape>(ToJoltVec3(halfExtents));
}

ShapeHierarchy ShapeFactory::CreateCapsuleShape(float halfHeight, float radius)
{
    return CreateShapeInternal<JPH::CapsuleShape>(halfHeight, radius);
}

ShapeHierarchy ShapeFactory::CreateCylinderShape(float halfHeight, float radius)
{
    return CreateShapeInternal<JPH::CylinderShape>(halfHeight, radius);
}
