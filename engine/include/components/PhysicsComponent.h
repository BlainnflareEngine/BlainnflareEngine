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
    PhysicsComponent(Entity entityIn, ComponentShapeType shapeTypeIn)
        : settings(entityIn, shapeTypeIn)
        , shapeType(shapeTypeIn) {};

    void UpdateShape(ComponentShapeType newType, JPH::Ref<JPH::Shape> shape)
    {
        shapeType = newType;
        shapePtr = shape;
    };

    JPH::Ref<JPH::Shape> GetShape()
    {
        return shapePtr;
    };

    ComponentShapeType GetShapeType()
    {
        return shapeType;
    }

    PhysicsComponentSettings settings;
    uuid parentId = {};
    JPH::BodyID bodyId = JPH::BodyID();
    Vec3 prevFrameScale = Vec3::One; // for rescale tracking
    bool controlParentTransform = true;

private:
    ComponentShapeType shapeType = ComponentShapeType::Empty;
    JPH::Ref<JPH::Shape> shapePtr = nullptr;
};
} // namespace Blainn