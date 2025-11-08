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

    Vec3 GetPosition();
    Quat GetRotation();
    Vec3 GetScale();

    Vec3 GetVelocity();
    float GetMaxLinearVelocity();
    Vec3 GetAngularVelocity();
    float GetMaxAngularVelocity();
    float GetGravityFactor();
    JPH::ObjectLayer GetObjectLayer();

private:
    JPH::BodyLockRead m_bodyLock;
    JPH::BodyInterface &m_bodyInterface;
    JPH::BodyID m_bodyId;
};

} // namespace Blainn