#include "pch.h"

#include "physics/BodyGetter.h"

#include "physics/Conversion.h"

using namespace Blainn;

Vec3 Blainn::BodyGetter::GetPosition()
{
    const JPH::Body &body = m_bodyLock.GetBody();
    return ToBlainnVec3(body.GetPosition());
}

Quat Blainn::BodyGetter::GetRotation()
{
    const JPH::Body &body = m_bodyLock.GetBody();
    return ToBlainnQuat(body.GetRotation());
}

JPH::RefConst<JPH::Shape> Blainn::BodyGetter::GetShape()
{
    const JPH::Body &body = m_bodyLock.GetBody();
    return body.GetShape();
}

Vec3 BodyGetter::GetVelocity()
{
    const JPH::Body &body = m_bodyLock.GetBody();
    return ToBlainnVec3(body.GetLinearVelocity());
}

float BodyGetter::GetMaxLinearVelocity()
{
    const JPH::Body &body = m_bodyLock.GetBody();
    return body.GetMotionProperties()->GetMaxLinearVelocity();
}

Vec3 BodyGetter::GetAngularVelocity()
{
    const JPH::Body &body = m_bodyLock.GetBody();
    return ToBlainnVec3(body.GetAngularVelocity());
}

float BodyGetter::GetMaxAngularVelocity()
{
    const JPH::Body &body = m_bodyLock.GetBody();
    return body.GetMotionProperties()->GetMaxAngularVelocity();
}

float BodyGetter::GetGravityFactor()
{
    const JPH::Body &body = m_bodyLock.GetBody();
    return body.GetMotionProperties()->GetGravityFactor();
}

JPH::ObjectLayer BodyGetter::GetObjectLayer()
{
    const JPH::Body &body = m_bodyLock.GetBody();
    return body.GetObjectLayer();
}