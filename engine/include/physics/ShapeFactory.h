#pragma once

#include "aliases.h"

#include "physics/PhysicsCreationSettings.h"

namespace Blainn
{
struct ShapeHierarchy
{
    JPH::Ref<JPH::Shape> childPtr = nullptr;
    JPH::Ref<JPH::Shape> shapePtr = nullptr;
};

class ShapeFactory
{
public:
    static eastl::optional<ShapeHierarchy> CreateShape(ShapeCreationSettings &settings);

private:
    ShapeFactory() = delete;

    static ShapeHierarchy CreateSphereShape(float radius);
    static ShapeHierarchy CreateBoxShape(Vec3 halfExtents);
    static ShapeHierarchy CreateCapsuleShape(float halfHeight, float radius);
    static ShapeHierarchy CreateCylinderShape(float halfHeight, float radius);

    // all shapes are scaled shapes
    template <typename ShapeSettingsType, typename... Args> static ShapeHierarchy CreateShapeInternal(Args... args)
    {
        ShapeSettingsType childSettings(args...);
        JPH::ShapeSettings::ShapeResult resChild = childSettings.Create();

        if (!resChild.IsValid())
        {
            BF_ERROR("failed to create jolt shape");
            return ShapeHierarchy{};
        }

        JPH::ScaledShapeSettings scaledShapeSettings(resChild.Get().GetPtr(), JPH::Vec3::sReplicate(1.0f));
        JPH::ShapeSettings::ShapeResult resScaled = scaledShapeSettings.Create();

        if (!resScaled.IsValid())
        {
            BF_ERROR("failed to create jolt shape");
            return ShapeHierarchy{};
        }

        return ShapeHierarchy{.childPtr = resChild.Get(), .shapePtr = resScaled.Get()};
    };
};
} // namespace Blainn