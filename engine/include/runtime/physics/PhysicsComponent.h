#pragma once

#include "EASTL/unique_ptr.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Body/Body.h"
#include "Jolt/Physics/Body/BodyID.h"

#include "aliases.h"


namespace Blainn
{
    class BodyBuilder;

    class PhysicsComponent
    {
    public:
        PhysicsComponent(uuid parentId, uuid componentId, BodyBuilder &builder);

        // TODO:
        // void SetVelocity(Vec3 velocity);
        // void SetAngularVelocity(Vec3 angularVelocity);
        // void ApplyForce(Vec3 force);
        // void ApplyTorque(Vec3 torque);
        // void SetPosition(Vec3 position);
        // void SetRotation(Quat rotation);

        uuid GetId() const;
    private:
        uuid m_id; // TODO: set zero uuid
        uuid m_parentId; // TODO: set zero uuid
        JPH::BodyID m_bodyId = JPH::BodyID(); // Default invalid body ID
        // some properties
    };
}