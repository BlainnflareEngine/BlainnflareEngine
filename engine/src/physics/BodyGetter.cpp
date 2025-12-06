#include "pch.h"

#include "physics/BodyGetter.h"

#include "physics/Conversion.h"

using namespace Blainn;

Vec3 Blainn::BodyGetter::GetPosition()
{
    return ToBlainnVec3(m_body.GetPosition());
}

Quat Blainn::BodyGetter::GetRotation()
{
    return ToBlainnQuat(m_body.GetRotation());
}

JPH::RefConst<JPH::Shape> Blainn::BodyGetter::GetShape()
{
    return m_body.GetShape();
}

Vec3 BodyGetter::GetVelocity()
{
    return ToBlainnVec3(m_body.GetLinearVelocity());
}

float BodyGetter::GetMaxLinearVelocity()
{
    return m_body.GetMotionProperties()->GetMaxLinearVelocity();
}

Vec3 BodyGetter::GetAngularVelocity()
{
    return ToBlainnVec3(m_body.GetAngularVelocity());
}

float BodyGetter::GetMaxAngularVelocity()
{
    return m_body.GetMotionProperties()->GetMaxAngularVelocity();
}

float BodyGetter::GetGravityFactor()
{
    return m_body.GetMotionProperties()->GetGravityFactor();
}

JPH::ObjectLayer BodyGetter::GetObjectLayer()
{
    return m_body.GetObjectLayer();
}

bool Blainn::BodyGetter::isTrigger()
{
    return m_body.IsSensor();
}