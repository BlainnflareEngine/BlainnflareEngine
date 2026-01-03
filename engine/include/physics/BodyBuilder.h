#pragma once

#include "aliases.h"

#include "physics/Conversion.h"
#include "physics/PhysicsTypes.h"

namespace Blainn
{
class BodyBuilder
{
public:
    BodyBuilder();

    BodyBuilder &SetMotionType(PhysicsComponentMotionType motionType);
    BodyBuilder &SetShape(JPH::Shape *shape);
    BodyBuilder &SetLayer(JPH::ObjectLayer layer);

    BodyBuilder &SetPosition(Vec3 vec);
    BodyBuilder &SetRotation(Quat quat);
    BodyBuilder &SetLinearVelocity(Vec3 vec);
    BodyBuilder &SetAngularVelocity(Vec3 vec);
    BodyBuilder &SetIsTrigger(bool isTrigger);
    BodyBuilder &SetGravityFactor(float factor);
    BodyBuilder &SetCollideKinematicVsNonDynamic(bool collide);

    JPH::BodyID Build(EActivation activate = EActivation::DontActivate);

private:
    JPH::BodyCreationSettings m_settings;
};
} // namespace Blainn