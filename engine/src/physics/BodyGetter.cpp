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
    if (m_body.IsStatic())
    {
        BF_ERROR("cannot get max linear velocity from static object");
        return 0.0f;
    }
    return m_body.GetMotionProperties()->GetMaxLinearVelocity();
}

Vec3 BodyGetter::GetAngularVelocity()
{
    return ToBlainnVec3(m_body.GetAngularVelocity());
}

float BodyGetter::GetMaxAngularVelocity()
{
    if (m_body.IsStatic())
    {
        BF_ERROR("cannot get max angular velocity from static object");
        return 0.0f;
    }
    return m_body.GetMotionProperties()->GetMaxAngularVelocity();
}

float BodyGetter::GetGravityFactor()
{
    if (m_body.IsStatic())
    {
        BF_ERROR("cannot get gravity factor from static object");
        return 1.0f;
    }
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


AABox Blainn::BodyGetter::GetShapeBoundingBox()
{
    const JPH::Body &body = m_bodyLock.GetBody();
    JPH::Vec3 scale = static_cast<const JPH::ScaledShape *>(body.GetShape())->GetScale();
    JPH::Mat44 comTransform = body.GetCenterOfMassTransform();
    return body.GetShape()->GetWorldSpaceBounds(comTransform, scale);
}

bool BodyGetter::isTrigger()
{
    return m_body.IsSensor();
}

bool Blainn::BodyGetter::collidesKinematicVsNonDynamic()
{
    return m_body.GetCollideKinematicVsNonDynamic();
}


AllowedDOFs Blainn::BodyGetter::GetAllowedDOFs()
{
    if (m_body.IsStatic())
    {
        return AllowedDOFs::All;
    }
    return m_body.GetMotionProperties()->GetAllowedDOFs();
}

eastl::optional<float> Blainn::BodyGetter::GetSphereShapeRadius()
{
    PhysicsComponent &component = PhysicsSubsystem::GetPhysicsComponentByBodyId(m_bodyId);
    if (component.GetShapeType() != ComponentShapeType::Sphere)
    {
        BF_ERROR("cannot get sphere radius - shape is not sphere");
        return eastl::nullopt;
    }

    JPH::ScaledShape *scaledShapePtr = static_cast<JPH::ScaledShape *>(component.GetShape().GetPtr());
    return static_cast<const JPH::SphereShape *>(scaledShapePtr->GetInnerShape())->GetRadius();
}

eastl::optional<Vec3> Blainn::BodyGetter::GetBoxShapeHalfExtents()
{
    PhysicsComponent &component = PhysicsSubsystem::GetPhysicsComponentByBodyId(m_bodyId);
    if (component.GetShapeType() != ComponentShapeType::Box)
    {
        BF_ERROR("cannot get box extents - shape is not box");
        return eastl::nullopt;
    }

    JPH::ScaledShape *scaledShapePtr = static_cast<JPH::ScaledShape *>(component.GetShape().GetPtr());
    return ToBlainnVec3(static_cast<const JPH::BoxShape *>(scaledShapePtr->GetInnerShape())->GetHalfExtent());
}


eastl::optional<eastl::pair<float, float>> Blainn::BodyGetter::GetCylinderShapeHalfHeightAndRadius()
{
    PhysicsComponent &component = PhysicsSubsystem::GetPhysicsComponentByBodyId(m_bodyId);
    if (component.GetShapeType() != ComponentShapeType::Cylinder)
    {
        BF_ERROR("cannot get cylinder height and radius - shape is not cylinder");
        return eastl::nullopt;
    }

    JPH::ScaledShape *scaledShapePtr = static_cast<JPH::ScaledShape *>(component.GetShape().GetPtr());
    const JPH::CylinderShape *shapePtr = static_cast<const JPH::CylinderShape *>(scaledShapePtr->GetInnerShape());
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

    JPH::ScaledShape *scaledShapePtr = static_cast<JPH::ScaledShape *>(component.GetShape().GetPtr());
    const JPH::CapsuleShape *shapePtr = static_cast<const JPH::CapsuleShape *>(scaledShapePtr->GetInnerShape());
    return eastl::optional<eastl::pair<float, float>>{{shapePtr->GetHalfHeightOfCylinder(), shapePtr->GetRadius()}};
}
