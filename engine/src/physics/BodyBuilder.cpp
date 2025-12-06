#include "pch.h"

#include "physics/BodyBuilder.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include "aliases.h"
#include "subsystems/PhysicsSubsystem.h"

using namespace Blainn;

BodyBuilder &BodyBuilder::SetMotionType(JPH::EMotionType motionType)
{
    m_settings.mMotionType = motionType;
    return *this;
}

BodyBuilder &BodyBuilder::SetShape(JPH::Shape *shape)
{
    m_settings.SetShape(shape);
    return *this;
}


BodyBuilder &Blainn::BodyBuilder::SetLayer(JPH::ObjectLayer layer)
{
    m_settings.mObjectLayer = layer;
    return *this;
}
BodyBuilder &BodyBuilder::SetPosition(Vec3 vec)
{
    m_settings.mPosition = ToJoltRVec3(vec);
    return *this;
}

BodyBuilder &BodyBuilder::SetRotation(Quat quat)
{
    m_settings.mRotation = ToJoltQuat(quat);
    return *this;
}

BodyBuilder &BodyBuilder::SetLinearVelocity(Vec3 vec)
{
    m_settings.mLinearVelocity = ToJoltVec3(vec);
    return *this;
}

BodyBuilder &BodyBuilder::SetAngularVelocity(Vec3 vec)
{
    m_settings.mAngularVelocity = ToJoltVec3(vec);
    return *this;
}

BodyBuilder &Blainn::BodyBuilder::SetIsTrigger(bool isTrigger)
{
    m_settings.mIsSensor = isTrigger;
    if (isTrigger)
    {
        m_settings.mCollideKinematicVsNonDynamic = true;
    }

    return *this;
}


BodyBuilder &Blainn::BodyBuilder::SetGravityFactor(float factor)
{
    m_settings.mGravityFactor = factor;
    return *this;
}

JPH::BodyID BodyBuilder::Build(JPH::EActivation activate /*= JPH::EActivation::Activate*/)
{
    // TODO: remove or change
    SetIsTrigger(true);

    m_settings.mAllowSleeping = false;
    JPH::BodyInterface &interf = PhysicsSubsystem::GetPhysicsSystem().GetBodyInterface();
    return interf.CreateAndAddBody(m_settings, activate);
}