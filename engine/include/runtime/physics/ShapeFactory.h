#pragma once

#include <EASTL/utility.h>

namespace JPH
{
    class SphereShape;
}

#include "common/aliases.h"

namespace Blainn
{
    class ShapeFactory
    {
        public:
            // TODO: static methods to create Jolt shapes
            static eastl::pair<uuid, JPH::SphereShape> CreateSphereShape(float radius);
        private:
            ShapeFactory() = delete;
    };
}