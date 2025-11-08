#include "pch.h"

#include "physics/BodyUpdater.h"

#include "physics/Conversion.h"

using namespace Blainn;

BodyUpdater &BodyUpdater::SetVelocity(Vec3 velocity)
{
    const JPH::Body &body = m_bodyLock.GetBody();
    m_bodyInterface.SetLinearVelocity(m_bodyId, ToJoltVec3(velocity));
    return *this;
}

BodyUpdater &Blainn::BodyUpdater::SetMaxLinearVelocity(float maxVelocity)
{
    const JPH::Body &body = m_bodyLock.GetBody();
    m_bodyInterface.SetMaxLinearVelocity(m_bodyId, maxVelocity);
    return *this;
}

BodyUpdater &Blainn::BodyUpdater::SetAngularVelocity(Vec3 angularVelocity)
{
    const JPH::Body &body = m_bodyLock.GetBody();
    m_bodyInterface.SetAngularVelocity(m_bodyId, ToJoltVec3(angularVelocity));
    return *this;
}

BodyUpdater &Blainn::BodyUpdater::SetMaxAngularVelocity(float maxAngularVelocity)
{
    const JPH::Body &body = m_bodyLock.GetBody();
    m_bodyInterface.SetMaxAngularVelocity(m_bodyId, maxAngularVelocity);
    return *this;
}

BodyUpdater &Blainn::BodyUpdater::SetGravityFactor(float gravityFactor)
{
    const JPH::Body &body = m_bodyLock.GetBody();
    m_bodyInterface.SetGravityFactor(m_bodyId, gravityFactor);
    return *this;
}

BodyUpdater &Blainn::BodyUpdater::SetObjectLayer(JPH::ObjectLayer layer)
{
    const JPH::Body &body = m_bodyLock.GetBody();
    m_bodyInterface.SetObjectLayer(m_bodyId, layer);
    return *this;
}

BodyUpdater &Blainn::BodyUpdater::AddVelocity(Vec3 deltaVelocity)
{
    const JPH::Body &body = m_bodyLock.GetBody();
    m_bodyInterface.AddLinearVelocity(m_bodyId, ToJoltVec3(deltaVelocity));
    return *this;
}

BodyUpdater &Blainn::BodyUpdater::AddImpulse(Vec3 impulse)
{
    const JPH::Body &body = m_bodyLock.GetBody();
    m_bodyInterface.AddImpulse(m_bodyId, ToJoltVec3(impulse));
    return *this;
}

BodyUpdater &Blainn::BodyUpdater::AddAngularImpulse(Vec3 angularImpulse)
{
    const JPH::Body &body = m_bodyLock.GetBody();
    m_bodyInterface.AddAngularImpulse(m_bodyId, ToJoltVec3(angularImpulse));
    return *this;
}

BodyUpdater &Blainn::BodyUpdater::AddForce(Vec3 force)
{
    const JPH::Body &body = m_bodyLock.GetBody();
    m_bodyInterface.AddForce(m_bodyId, ToJoltVec3(force));
    return *this;
}
