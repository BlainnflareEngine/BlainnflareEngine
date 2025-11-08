#pragma once

#include <cassert>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Body/BodyLock.h>
#include <Jolt/Physics/Body/BodyLockInterface.h>

#include "aliases.h"

namespace Blainn
{

class BodyGetter
{
public:
    BodyGetter(const JPH::BodyLockInterfaceLocking &bodyLockInterface, JPH::BodyInterface &bodyInterface,
               JPH::BodyID bodyId)
        : m_bodyInterface(bodyInterface)
        , m_bodyId(bodyId)
        , m_bodyLock(bodyLockInterface, bodyId)
    {
        assert(m_bodyLock.Succeeded());
    }
    ~BodyGetter() = default;

    Vec3 GetVelocity(Vec3 velocity);
    float GetMaxLinearVelocity(float maxVelocity);
    Vec3 GetAngularVelocity(Vec3 angularVelocity);
    float GetMaxAngularVelocity(float maxAngularVelocity);
    float GetGravityFactor(float gravityFactor);
    JPH::ObjectLayer GetObjectLayer(JPH::ObjectLayer layer);

private:
    JPH::BodyLockRead m_bodyLock;
    JPH::BodyInterface &m_bodyInterface;
    JPH::BodyID m_bodyId;
};

} // namespace Blainn