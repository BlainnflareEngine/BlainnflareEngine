#include "pch.h"

#include "physics/ShapeFactory.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>

#include "physics/Conversion.h"
#include "tools/random.h"

using namespace Blainn;

// TODO: do we need shared pointers here?

eastl::pair<uuid, eastl::shared_ptr<JPH::Shape>> ShapeFactory::CreateSphereShape(float radius)
{
    uuid id = Rand::getRandomUUID();
    auto shapePtr = eastl::make_shared<JPH::SphereShape>(radius);

    return eastl::pair<uuid, eastl::shared_ptr<JPH::Shape>>{eastl::move(id), shapePtr};
}

eastl::pair<uuid, eastl::shared_ptr<JPH::Shape>> ShapeFactory::CreateBoxShape(Vec3 halfExtents)
{
    uuid id = Rand::getRandomUUID();
    auto shapePtr = eastl::make_shared<JPH::BoxShape>(ToJoltVec3(halfExtents));

    return eastl::pair<uuid, eastl::shared_ptr<JPH::Shape>>{eastl::move(id), shapePtr};
}

eastl::pair<uuid, eastl::shared_ptr<JPH::Shape>> ShapeFactory::CreateCapsuleShape(float halfHeight, float radius)
{
    uuid id = Rand::getRandomUUID();
    auto shapePtr = eastl::make_shared<JPH::CapsuleShape>(halfHeight, radius);

    return eastl::pair<uuid, eastl::shared_ptr<JPH::Shape>>{eastl::move(id), shapePtr};
}

eastl::pair<uuid, eastl::shared_ptr<JPH::Shape>> ShapeFactory::CreateCylinderShape(float halfHeight, float radius)
{
    uuid id = Rand::getRandomUUID();
    auto shapePtr = eastl::make_shared<JPH::CylinderShape>(halfHeight, radius);

    return eastl::pair<uuid, eastl::shared_ptr<JPH::Shape>>{eastl::move(id), shapePtr};
}
