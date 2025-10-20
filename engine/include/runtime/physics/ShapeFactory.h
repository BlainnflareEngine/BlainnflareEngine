#pragma once

#include <EASTL/utility.h>
#include<EASTL/unordered_map.h>

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Collision/Shape/Shape.h"

#include "aliases.h"

namespace Blainn
{
    class ShapeFactory
    {
        public:
            // TODO: static methods to create Jolt shapes
            static eastl::pair<uuid, eastl::shared_ptr<JPH::Shape>> CreateSphereShape(float radius);
        private:
            ShapeFactory() = delete;
            static eastl::unordered_map<uuid, eastl::shared_ptr<JPH::Shape>, std::hash<uuid>> m_shapes;
    };
}