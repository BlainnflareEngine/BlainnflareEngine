#include "pch.h"
#include "runtime/physics/ShapeFactory.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Collision/Shape/SphereShape.h"

#include "tools/random.h"

using namespace Blainn;

eastl::pair<uuid, eastl::shared_ptr<JPH::Shape>> Blainn::ShapeFactory::CreateSphereShape(float radius)
{
    uuid id = Rand::getRandomUUID();
    auto shapePtr = eastl::make_shared<JPH::SphereShape>(radius);
    if (m_shapes.contains(id))
    {
        // TODO: log error
    }

    return eastl::pair<uuid, eastl::shared_ptr<JPH::Shape>>{eastl::move(id), shapePtr};
}
