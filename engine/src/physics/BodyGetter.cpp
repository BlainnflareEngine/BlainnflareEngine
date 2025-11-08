#include "pch.h"

#include "physics/BodyGetter.h"

#include "physics/Conversion.h"

using namespace Blainn;

Vec3 BodyGetter::GetVelocity(Vec3 velocity)
{
    const JPH::Body &body = m_bodyLock.GetBody();
    return ToBlainnVec3(body.GetLinearVelocity());
}

float BodyGetter::GetMaxLinearVelocity(float maxVelocity)
{
    const JPH::Body &body = m_bodyLock.GetBody();
    return body.GetMotionProperties()->GetMaxLinearVelocity();
}

Vec3 BodyGetter::GetAngularVelocity(Vec3 angularVelocity)
{
    const JPH::Body &body = m_bodyLock.GetBody();
    return ToBlainnVec3(body.GetAngularVelocity());
}

float BodyGetter::GetMaxAngularVelocity(float maxAngularVelocity)
{
    const JPH::Body &body = m_bodyLock.GetBody();
    return body.GetMotionProperties()->GetMaxAngularVelocity();
}

float BodyGetter::GetGravityFactor(float gravityFactor)
{
    const JPH::Body &body = m_bodyLock.GetBody();
    return body.GetMotionProperties()->GetGravityFactor();
}

JPH::ObjectLayer BodyGetter::GetObjectLayer(JPH::ObjectLayer layer)
{
    const JPH::Body &body = m_bodyLock.GetBody();
    return body.GetObjectLayer();
}