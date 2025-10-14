#pragma once

#include "EASTL/shared_ptr.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Body/BodyCreationSettings.h"
#include "Jolt/Physics/EActivation.h"

#include "aliases.h"
#include "tools/conversion.h"

namespace Blainn
{
    class BodyBuilder{
        public:
            BodyBuilder() = default;

            BodyBuilder& SetMotionType(JPH::EMotionType motionType);
            BodyBuilder& SetShape(eastl::shared_ptr<JPH::Shape> shape);

            BodyBuilder& SetPosition(Vec3 vec);
            BodyBuilder& SetRotation(Quat quat);
            BodyBuilder& SetLinearVelocity(Vec3 vec);
            BodyBuilder& SetAngularVelocity(Vec3 vec);

            // const JPH::BodyCreationSettings& GetBodyCreationSettings() const;
            JPH::BodyID Build(JPH::EActivation activate = JPH::EActivation::Activate);

        private:
            JPH::BodyCreationSettings m_settings;
    };
}