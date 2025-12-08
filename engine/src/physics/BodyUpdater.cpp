#include "pch.h"

#include "physics/BodyUpdater.h"

#include "subsystems/PhysicsSubsystem.h"
#include "physics/Conversion.h"
#include "physics/ShapeFactory.h"

using namespace Blainn;

BodyUpdater &BodyUpdater::SetPosition(Vec3 position, JPH::EActivation activation)
{
    m_bodyInterface.SetPosition(m_bodyId, ToJoltRVec3(position), activation);
    return *this;
}

BodyUpdater &BodyUpdater::SetRotation(Quat rotation, JPH::EActivation activation)
{
    m_bodyInterface.SetRotation(m_bodyId, ToJoltQuat(rotation), activation);
    return *this;
}

// some cringe
BodyUpdater &BodyUpdater::SetScale(Vec3 scale, Vec3 prevScale)
{
    JPH::Vec3 newShapeScale(ToJoltVec3(scale / prevScale));

    JPH::BodyLockWrite bodyLock(m_bodyLockInterface, m_bodyId);
    JPH::Body &body = bodyLock.GetBody();
    JPH::Shape::ShapeResult res = body.GetShape()->ScaleShape(newShapeScale);
    return *this;
}

BodyUpdater &BodyUpdater::SetVelocity(Vec3 velocity)
{
    m_bodyInterface.SetLinearVelocity(m_bodyId, ToJoltVec3(velocity));
    return *this;
}

BodyUpdater &BodyUpdater::SetMaxLinearVelocity(float maxVelocity)
{
    m_bodyInterface.SetMaxLinearVelocity(m_bodyId, maxVelocity);
    return *this;
}

BodyUpdater &BodyUpdater::SetAngularVelocity(Vec3 angularVelocity)
{
    m_bodyInterface.SetAngularVelocity(m_bodyId, ToJoltVec3(angularVelocity));
    return *this;
}

BodyUpdater &BodyUpdater::SetMaxAngularVelocity(float maxAngularVelocity)
{
    m_bodyInterface.SetMaxAngularVelocity(m_bodyId, maxAngularVelocity);
    return *this;
}

BodyUpdater &BodyUpdater::SetGravityFactor(float gravityFactor)
{
    m_bodyInterface.SetGravityFactor(m_bodyId, gravityFactor);
    return *this;
}

BodyUpdater &BodyUpdater::SetObjectLayer(JPH::ObjectLayer layer)
{
    m_bodyInterface.SetObjectLayer(m_bodyId, layer);
    return *this;
}

BodyUpdater &BodyUpdater::AddVelocity(Vec3 deltaVelocity)
{
    m_bodyInterface.AddLinearVelocity(m_bodyId, ToJoltVec3(deltaVelocity));
    return *this;
}

BodyUpdater &BodyUpdater::AddImpulse(Vec3 impulse)
{
    m_bodyInterface.AddImpulse(m_bodyId, ToJoltVec3(impulse));
    return *this;
}

BodyUpdater &BodyUpdater::AddAngularImpulse(Vec3 angularImpulse)
{
    m_bodyInterface.AddAngularImpulse(m_bodyId, ToJoltVec3(angularImpulse));
    return *this;
}

BodyUpdater &BodyUpdater::AddForce(Vec3 force)
{
    m_bodyInterface.AddForce(m_bodyId, ToJoltVec3(force));
    return *this;
}

BodyUpdater &BodyUpdater::ReplaceBodyShape(ShapeCreationSettings &settings)
{
    eastl::optional<ShapeHierarchy> hierarchy = ShapeFactory::CreateShape(settings);
    if (!hierarchy.has_value())
    {
        BF_ERROR("Error in replacing physics body shape");
        return *this;
    }

    PhysicsComponent &component = PhysicsSubsystem::GetPhysicsComponentByBodyId(m_bodyId);
    component.shapeHierarchy = eastl::move(hierarchy.value());

    return *this;
}

BodyUpdater &BodyUpdater::SetMotionType(PhysicsComponentMotionType motionType,
                                        EActivation activation /*= EActivation::Activate*/)
{
    m_bodyInterface.SetMotionType(m_bodyId, motionType, activation);
    return *this;
}

BodyUpdater &BodyUpdater::SetSphereShapeSettings(float radius)
{
    const ComponentShapeType sphereShapeType = ComponentShapeType::Sphere;

    PhysicsComponent &component = PhysicsSubsystem::GetPhysicsComponentByBodyId(m_bodyId);
    if (static_cast<ComponentShapeType>(component.shapeHierarchy.childPtr->GetSubType()) != sphereShapeType)
    {
        BF_ERROR("shape is not sphere, cannot set sphere settings");
        return *this;
    }

    ShapeCreationSettings newShapeSettings(sphereShapeType);
    newShapeSettings.radius = radius;
    ReplaceBodyShape(newShapeSettings);

    return *this;
}

BodyUpdater &Blainn::BodyUpdater::SetBoxShapeSettings(Vec3 halfExtents)
{
    const ComponentShapeType boxShapeType = ComponentShapeType::Box;

    PhysicsComponent &component = PhysicsSubsystem::GetPhysicsComponentByBodyId(m_bodyId);
    if (static_cast<ComponentShapeType>(component.shapeHierarchy.childPtr->GetSubType()) != boxShapeType)
    {
        BF_ERROR("shape is not box, cannot set box settings");
        return *this;
    }

    ShapeCreationSettings newShapeSettings(boxShapeType);
    newShapeSettings.halfExtents = halfExtents;
    ReplaceBodyShape(newShapeSettings);

    return *this;
}

BodyUpdater &Blainn::BodyUpdater::SetCapsuleShapeSettings(float halfCylinderHeight, float radius)
{
    const ComponentShapeType capsuleShapeType = ComponentShapeType::Capsule;

    PhysicsComponent &component = PhysicsSubsystem::GetPhysicsComponentByBodyId(m_bodyId);
    if (static_cast<ComponentShapeType>(component.shapeHierarchy.childPtr->GetSubType()) != capsuleShapeType)
    {
        BF_ERROR("shape is not capsule, cannot set capsule settings");
        return *this;
    }

    ShapeCreationSettings newShapeSettings(capsuleShapeType);
    newShapeSettings.halfCylinderHeight = halfCylinderHeight;
    newShapeSettings.radius = radius;
    ReplaceBodyShape(newShapeSettings);

    return *this;
}

BodyUpdater &Blainn::BodyUpdater::SetCylinderShapeSettings(float halfCylinderHeight, float radius)
{
    const ComponentShapeType cylinderShapeType = ComponentShapeType::Cylinder;

    PhysicsComponent &component = PhysicsSubsystem::GetPhysicsComponentByBodyId(m_bodyId);
    if (static_cast<ComponentShapeType>(component.shapeHierarchy.childPtr->GetSubType()) != cylinderShapeType)
    {
        BF_ERROR("shape is not cylinder, cannot set cylinder settings");
        return *this;
    }

    ShapeCreationSettings newShapeSettings(cylinderShapeType);
    newShapeSettings.halfCylinderHeight = halfCylinderHeight;
    newShapeSettings.radius = radius;
    ReplaceBodyShape(newShapeSettings);

    return *this;
}
