#pragma once

#include <cassert>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Body/BodyLock.h>
#include <Jolt/Physics/Body/BodyLockInterface.h>

#include "aliases.h"
#include "helpers.h"

#include "physics/PhysicsTypes.h"

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
        , m_body(m_bodyLock.GetBody())
    {
        assert(m_bodyLock.Succeeded());
    }
    NO_COPY_DEFAULT_MOVE(BodyGetter);
    ~BodyGetter() = default;

    Vec3 GetPosition();
    Quat GetRotation();
    Vec3 GetScale(); // TODO:?

    JPH::RefConst<JPH::Shape> GetShape();

    Vec3 GetVelocity();
    float GetMaxLinearVelocity();
    Vec3 GetAngularVelocity();
    float GetMaxAngularVelocity();
    float GetGravityFactor();
    ObjectLayer GetObjectLayer();
    bool isTrigger();

private:
    JPH::BodyLockRead m_bodyLock;
    JPH::BodyInterface &m_bodyInterface;
    JPH::BodyID m_bodyId;
    const JPH::Body &m_body;
};

} // namespace Blainn