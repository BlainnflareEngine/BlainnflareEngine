#pragma once

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Body/BodyCreationSettings.h"


namespace Blainn
{
    class BodyBuilder{
        public:
            BodyBuilder() = default;

            BodyBuilder& setMotionType(JPH::EMotionType motionType);
            BodyBuilder& setShape(const JPH::Shape shape);
            BodyBuilder& setShapeSettings(const JPH::ShapeSettings *shapeSettings);
            // TODO: replace with Blainn types
            BodyBuilder& setPosition(JPH::RVec3 rvec);
            BodyBuilder& setRotation(JPH::Quat quat);
            BodyBuilder& setLinearVelocity(JPH::Vec3 vec);
            BodyBuilder& setAngularVelocity(JPH::Vec3 vec);

            const JPH::BodyCreationSettings& getBodySettings() const;

        private:
            JPH::BodyCreationSettings settings;
    };
}