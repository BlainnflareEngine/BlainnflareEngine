#include "common/pch.h"
#include "ShapeFactory.h"

#include "Jolt/Physics/Collision/Shape/SphereShape.h"

using namespace Blainn;

eastl::pair<uuid, JPH::SphereShape> Blainn::ShapeFactory::CreateSphereShape(float radius)
{
    return eastl::pair<uuid, JPH::SphereShape>();
}
