#include "pch.h"

#include "physics/BodyUpdater.h"

#include "physics/Conversion.h"

using namespace Blainn;

BodyUpdater &Blainn::BodyUpdater::SetPosition(Vec3 position, JPH::EActivation activation)
{
    m_bodyInterface.SetPosition(m_bodyId, ToJoltRVec3(position), activation);
    return *this;
}

BodyUpdater &Blainn::BodyUpdater::SetRotation(Quat rotation, JPH::EActivation activation)
{
    m_bodyInterface.SetRotation(m_bodyId, ToJoltQuat(rotation), activation);
    return *this;
}

BodyUpdater &Blainn::BodyUpdater::SetScale(Vec3 scale)
{
    JPH::BodyLockWrite bodyLock(m_bodyLockInterface, m_bodyId);
    JPH::Body &body = bodyLock.GetBody();
    // TODO: оно возвращает новый shape?
    JPH::Shape::ShapeResult res = body.GetShape()->ScaleShape(ToJoltVec3(scale));
    return *this;
}

BodyUpdater &BodyUpdater::SetVelocity(Vec3 velocity)
{
    m_bodyInterface.SetLinearVelocity(m_bodyId, ToJoltVec3(velocity));
    return *this;
}

BodyUpdater &Blainn::BodyUpdater::SetMaxLinearVelocity(float maxVelocity)
{
    m_bodyInterface.SetMaxLinearVelocity(m_bodyId, maxVelocity);
    return *this;
}

BodyUpdater &Blainn::BodyUpdater::SetAngularVelocity(Vec3 angularVelocity)
{
    m_bodyInterface.SetAngularVelocity(m_bodyId, ToJoltVec3(angularVelocity));
    return *this;
}

BodyUpdater &Blainn::BodyUpdater::SetMaxAngularVelocity(float maxAngularVelocity)
{
    m_bodyInterface.SetMaxAngularVelocity(m_bodyId, maxAngularVelocity);
    return *this;
}

BodyUpdater &Blainn::BodyUpdater::SetGravityFactor(float gravityFactor)
{
    m_bodyInterface.SetGravityFactor(m_bodyId, gravityFactor);
    return *this;
}

BodyUpdater &Blainn::BodyUpdater::SetObjectLayer(JPH::ObjectLayer layer)
{
    m_bodyInterface.SetObjectLayer(m_bodyId, layer);
    return *this;
}

BodyUpdater &Blainn::BodyUpdater::AddVelocity(Vec3 deltaVelocity)
{
    m_bodyInterface.AddLinearVelocity(m_bodyId, ToJoltVec3(deltaVelocity));
    return *this;
}

BodyUpdater &Blainn::BodyUpdater::AddImpulse(Vec3 impulse)
{
    m_bodyInterface.AddImpulse(m_bodyId, ToJoltVec3(impulse));
    return *this;
}

BodyUpdater &Blainn::BodyUpdater::AddAngularImpulse(Vec3 angularImpulse)
{
    m_bodyInterface.AddAngularImpulse(m_bodyId, ToJoltVec3(angularImpulse));
    return *this;
}

BodyUpdater &Blainn::BodyUpdater::AddForce(Vec3 force)
{
    m_bodyInterface.AddForce(m_bodyId, ToJoltVec3(force));
    return *this;
}
