#pragma once

#include "aliases.h"

#include "physics/PhysicsCreationSettings.h"

namespace Blainn
{
class ShapeFactory
{
public:
    static eastl::optional<JPH::Ref<JPH::Shape>> CreateShape(ShapeCreationSettings &settings);

private:
    ShapeFactory() = delete;

    static JPH::Ref<JPH::Shape> CreateSphereShape(float radius);
    static JPH::Ref<JPH::Shape> CreateBoxShape(Vec3 halfExtents);
    static JPH::Ref<JPH::Shape> CreateCapsuleShape(float halfHeight, float radius);
    static JPH::Ref<JPH::Shape> CreateCylinderShape(float halfHeight, float radius);

    // all shapes are scaled shapes
    template <typename ShapeSettingsType, typename... Args>
    static JPH::Ref<JPH::Shape> CreateShapeInternal(Args... args)
    {
        ShapeSettingsType childSettings(args...);
        JPH::ShapeSettings::ShapeResult resChild = childSettings.Create();

        if (!resChild.IsValid())
        {
            BF_ERROR("failed to create jolt shape");
            return eastl::nullopt;
        }

        JPH::ScaledShapeSettings scaledShapeSettings(resChild.Get().GetPtr(), JPH::Vec3::sReplicate(1.0f));
        JPH::ShapeSettings::ShapeResult resScaled = scaledShapeSettings.Create();

        if (!resScaled.IsValid())
        {
            BF_ERROR("failed to create jolt shape");
            return eastl::nullopt;
        }

        return resScaled.Get();
    };
};
} // namespace Blainn