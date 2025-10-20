#include "pch.h"
#include "runtime/physics/BodyBuilder.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Body/Body.h"
#include "Jolt/Physics/Body/BodyInterface.h"
#include "Jolt/Physics/PhysicsSystem.h"

#include "aliases.h"

#include "subsystems/PhysicsSubsystem.h"

using namespace Blainn;

BodyBuilder &BodyBuilder::SetMotionType(JPH::EMotionType motionType)
{
    m_settings.mMotionType = motionType;
    return *this;
}

BodyBuilder &BodyBuilder::SetShape(eastl::shared_ptr<JPH::Shape> shape)
{
    m_settings.SetShape(shape.get());
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

JPH::BodyID BodyBuilder::Build(JPH::EActivation activate /*= JPH::EActivation::Activate*/)
{
    JPH::BodyInterface & interf = PhysicsSubsystem::GetPhysicsSystem().GetBodyInterface();
    return interf.CreateAndAddBody(m_settings, activate);
}