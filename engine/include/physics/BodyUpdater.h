#pragma once

#include <cassert>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Body/BodyLock.h>
#include <Jolt/Physics/Body/BodyLockInterface.h>

#include "aliases.h"
#include "helpers.h"

namespace Blainn
{

// хотел сделать с одним локом, но увы, будет лочить на каждый set
class BodyUpdater
{
public:
    BodyUpdater(const JPH::BodyLockInterfaceLocking &bodyLockInterface, JPH::BodyInterface &bodyInterface,
                JPH::BodyID bodyId)
        : m_bodyLockInterface(bodyLockInterface)
        , m_bodyInterface(bodyInterface)
        , m_bodyId(bodyId)
    //, m_bodyLock(bodyLockInterface, bodyId)
    {
        // assert(m_bodyLock.Succeeded());
    }
    NO_COPY_DEFAULT_MOVE(BodyUpdater);
    ~BodyUpdater() = default;

    BodyUpdater &SetPosition(Vec3 position, JPH::EActivation activation = JPH::EActivation::Activate);
    BodyUpdater &SetRotation(Quat rotation, JPH::EActivation activation = JPH::EActivation::Activate);
    BodyUpdater &SetScale(Vec3 scale, Vec3 prevScale);

    BodyUpdater &SetVelocity(Vec3 velocity);
    BodyUpdater &SetMaxLinearVelocity(float maxVelocity);
    BodyUpdater &SetAngularVelocity(Vec3 angularVelocity);
    BodyUpdater &SetMaxAngularVelocity(float maxAngularVelocity);
    BodyUpdater &SetGravityFactor(float gravityFactor);
    BodyUpdater &SetObjectLayer(JPH::ObjectLayer layer);

    BodyUpdater &AddVelocity(Vec3 deltaVelocity);
    BodyUpdater &AddImpulse(Vec3 impulse);
    BodyUpdater &AddAngularImpulse(Vec3 angularImpulse);
    BodyUpdater &AddForce(Vec3 force);

private:
    // JPH::BodyLockWrite m_bodyLock;
    const JPH::BodyLockInterfaceLocking &m_bodyLockInterface;
    JPH::BodyInterface &m_bodyInterface;
    JPH::BodyID m_bodyId;
};

} // namespace Blainn