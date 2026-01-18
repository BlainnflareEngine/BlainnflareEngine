#include "pch.h"

#include "physics/ShapeFactory.h"

#include "physics/Conversion.h"

using namespace Blainn;


eastl::optional<JPH::Ref<JPH::Shape>> ShapeFactory::CreateShape(ShapeCreationSettings &settings)
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
        return eastl::nullopt;
    }
}

eastl::optional<JPH::Ref<JPH::Shape>> ShapeFactory::CreateSphereShape(float radius)
{
    return CreateShapeInternal<JPH::SphereShapeSettings>(radius);
}

eastl::optional<JPH::Ref<JPH::Shape>> ShapeFactory::CreateBoxShape(Vec3 halfExtents)
{
    return CreateShapeInternal<JPH::BoxShapeSettings>(ToJoltVec3(halfExtents));
}

eastl::optional<JPH::Ref<JPH::Shape>> ShapeFactory::CreateCapsuleShape(float halfHeight, float radius)
{
    return CreateShapeInternal<JPH::CapsuleShapeSettings>(halfHeight, radius);
}

eastl::optional<JPH::Ref<JPH::Shape>> ShapeFactory::CreateCylinderShape(float halfHeight, float radius)
{
    return CreateShapeInternal<JPH::CylinderShapeSettings>(halfHeight, radius);
}
