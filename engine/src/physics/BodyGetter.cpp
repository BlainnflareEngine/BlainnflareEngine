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

eastl::optional<float> Blainn::BodyGetter::GetSphereShapeRadius()
{
    PhysicsComponent &component = PhysicsSubsystem::GetPhysicsComponentByBodyId(m_bodyId);
    if (component.GetShapeType() != ComponentShapeType::Sphere)
    {
        BF_ERROR("cannot get sphere radius - shape is not sphere");
        return eastl::nullopt;
    }

    return static_cast<JPH::SphereShape *>(component.GetHierarchy().childPtr.GetPtr())->GetRadius();
}

eastl::optional<Vec3> Blainn::BodyGetter::GetBoxShapeHalfExtents()
{
    PhysicsComponent &component = PhysicsSubsystem::GetPhysicsComponentByBodyId(m_bodyId);
    if (component.GetShapeType() != ComponentShapeType::Box)
    {
        BF_ERROR("cannot get box extents - shape is not box");
        return eastl::nullopt;
    }

    return ToBlainnVec3(static_cast<JPH::BoxShape *>(component.GetHierarchy().childPtr.GetPtr())->GetHalfExtent());
}


eastl::optional<eastl::pair<float, float>> Blainn::BodyGetter::GetCylinderShapeHalfHeightAndRadius()
{
    PhysicsComponent &component = PhysicsSubsystem::GetPhysicsComponentByBodyId(m_bodyId);
    if (component.GetShapeType() != ComponentShapeType::Cylinder)
    {
        BF_ERROR("cannot get cylinder height and radius - shape is not cylinder");
        return eastl::nullopt;
    }

    JPH::CylinderShape *shapePtr = static_cast<JPH::CylinderShape *>(component.GetHierarchy().childPtr.GetPtr());
    return eastl::optional<eastl::pair<float, float>>{{shapePtr->GetHalfHeight(), shapePtr->GetRadius()}};
}

eastl::optional<eastl::pair<float, float>> Blainn::BodyGetter::GetCapsuleShapeHalfHeightAndRadius()
{
    PhysicsComponent &component = PhysicsSubsystem::GetPhysicsComponentByBodyId(m_bodyId);
    if (component.GetShapeType() != ComponentShapeType::Capsule)
    {
        BF_ERROR("cannot get capsule height and radius - shape is not capsule");
        return eastl::nullopt;
    }

    JPH::CapsuleShape *shapePtr = static_cast<JPH::CapsuleShape *>(component.GetHierarchy().childPtr.GetPtr());
    return eastl::optional<eastl::pair<float, float>>{{shapePtr->GetHalfHeightOfCylinder(), shapePtr->GetRadius()}};
}
