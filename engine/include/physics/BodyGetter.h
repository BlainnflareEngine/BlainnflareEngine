#pragma once

#include "aliases.h"
#include "helpers.h"

#include "physics/PhysicsTypes.h"

namespace Blainn
{

class BodyGetter
{
public:
    BodyGetter(const JPH::BodyLockInterfaceLocking &bodyLockInterface, JPH::BodyInterface &bodyInterface,
               JPH::BodyID bodyId)
        : m_bodyInterface(bodyInterface)
        , m_bodyId(bodyId)
        , m_bodyLock(bodyLockInterface, bodyId)
        , m_body(m_bodyLock.GetBody())
    {
        assert(m_bodyLock.Succeeded());
    }
    NO_COPY_DEFAULT_MOVE(BodyGetter);
    ~BodyGetter() = default;

    Vec3 GetPosition();
    Quat GetRotation();
    Vec3 GetScale(); // TODO:?

    JPH::RefConst<JPH::Shape> GetShape();
    ComponentShapeType GetShapeType();

    Vec3 GetVelocity();
    float GetMaxLinearVelocity();
    Vec3 GetAngularVelocity();
    float GetMaxAngularVelocity();
    float GetGravityFactor();
    bool isTrigger();

    ObjectLayer GetObjectLayer();
    PhysicsComponentMotionType GetMotionType();

    eastl::optional<float> GetSphereShapeRadius();
    eastl::optional<Vec3> GetBoxShapeHalfExtents();
    eastl::optional<eastl::pair<float, float>> GetCylinderShapeHalfHeightAndRadius();
    eastl::optional<eastl::pair<float, float>> GetCapsuleShapeHalfHeightAndRadius();

private:
    JPH::BodyLockRead m_bodyLock;
    JPH::BodyInterface &m_bodyInterface;
    JPH::BodyID m_bodyId;
    const JPH::Body &m_body;
};

} // namespace Blainn