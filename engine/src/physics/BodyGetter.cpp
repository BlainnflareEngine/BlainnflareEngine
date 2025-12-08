#include "pch.h"

#include "subsystems/PhysicsSubsystem.h"
#include "physics/BodyGetter.h"
#include "physics/Conversion.h"

using namespace Blainn;

Vec3 BodyGetter::GetPosition()
{
    return ToBlainnVec3(m_body.GetPosition());
}

Quat BodyGetter::GetRotation()
{
    return ToBlainnQuat(m_body.GetRotation());
}

JPH::RefConst<JPH::Shape> BodyGetter::GetShape()
{
    return m_body.GetShape();
}

ComponentShapeType BodyGetter::GetShapeType()
{
    PhysicsComponent &component = PhysicsSubsystem::GetPhysicsComponentByBodyId(m_bodyId);
    return component.GetShapeType();
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

PhysicsComponentMotionType BodyGetter::GetMotionType()
{
    return m_body.GetMotionType();
}

bool BodyGetter::isTrigger()
{
    return m_body.IsSensor();
}