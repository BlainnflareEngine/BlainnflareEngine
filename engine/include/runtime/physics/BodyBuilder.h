#pragma once

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Body/BodyCreationSettings.h"


namespace Blainn
{
    class BodyBuilder{
        public:
            BodyBuilder() = default;

            BodyBuilder& SetMotionType(JPH::EMotionType motionType);
            BodyBuilder& SetShape(const JPH::Shape shape);
            BodyBuilder& SetShapeSettings(const JPH::ShapeSettings *shapeSettings);
            // TODO: replace with Blainn types
            BodyBuilder& SetPosition(JPH::RVec3 rvec);
            BodyBuilder& SetRotation(JPH::Quat quat);
            BodyBuilder& SetLinearVelocity(JPH::Vec3 vec);
            BodyBuilder& SetAngularVelocity(JPH::Vec3 vec);

            const JPH::BodyCreationSettings& GetBodyCreationSettings() const;
            JPH::Body Build();

        private:
            JPH::BodyCreationSettings m_settings;
    };
}