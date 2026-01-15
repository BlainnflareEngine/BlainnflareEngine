#pragma once

#include "aliases.h"
#include "physics/PhysicsTypes.h"
#include "physics/ShapeFactory.h"

namespace eastl
{
template <> struct hash<JPH::BodyID>
{
    size_t operator()(const JPH::BodyID &bodyId) const EA_NOEXCEPT
    {
        return std::hash<JPH::BodyID>{}(bodyId); // must return size_t
    }
};
} // namespace eastl

namespace Blainn
{
struct PhysicsComponent
{
    void UpdateShape(ComponentShapeType newType, ShapeHierarchy newHierarchy)
    {
        shapeType = newType;
        shapeHierarchy = newHierarchy;
    };

    const ShapeHierarchy &GetHierarchy()
    {
        return shapeHierarchy;
    };

    ComponentShapeType GetShapeType()
    {
        return shapeType;
    }

    uuid parentId = {};
    JPH::BodyID bodyId = JPH::BodyID();
    Vec3 prevFrameScale = Vec3::One; // for rescale tracking
    bool controlParentTransform = true;

private:
    ComponentShapeType shapeType = ComponentShapeType::Empty;
    ShapeHierarchy shapeHierarchy = {}; // we may not store this
};
} // namespace Blainn