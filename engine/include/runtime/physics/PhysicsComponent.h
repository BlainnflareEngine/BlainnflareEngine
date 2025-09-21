#pragma once

#include "aliases.h"

#include "EASTL/unique_ptr.h"

namespace JPH {
    class Body;
}

namespace Blainn
{
    class BodyBuilder;

    class PhysicsComponent
    {
    public:
        PhysicsComponent(uuid parentId, uuid componentId, const BodyBuilder &builder);
    private:
        uuid m_id;
        uuid m_parentId;
        eastl::unique_ptr<JPH::Body> m_body;
        // some properties
    };
}