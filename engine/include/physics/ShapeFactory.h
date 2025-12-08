#pragma once

#include "aliases.h"

#include "physics/PhysicsCreationSettings.h"

namespace Blainn
{
struct ShapeHierarchy
{
    eastl::unique_ptr<JPH::Shape> shapePtr = nullptr;
    eastl::unique_ptr<JPH::Shape> childPtr = nullptr; // one for now but in must be vector
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
    template <typename ShapeType, typename... Args> static ShapeHierarchy CreateShapeInternal(Args... args)
    {
        ShapeHierarchy hierarchy;
        hierarchy.childPtr = eastl::make_unique<ShapeType>(args...);
        hierarchy.shapePtr =
            eastl::make_unique<JPH::ScaledShape>(hierarchy.childPtr.get(), JPH::Vec3::sReplicate(1.0f));
        return hierarchy;
    };
};
} // namespace Blainn