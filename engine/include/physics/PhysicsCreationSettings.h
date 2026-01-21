#pragma once

#include "aliases.h"

#include "physics/Layers.h"
#include "physics/PhysicsTypes.h"
#include "scene/Entity.h"

namespace Blainn
{
struct ShapeCreationSettings
{
    ShapeCreationSettings(ComponentShapeType shapeTypeIn)
        : shapeType(shapeTypeIn) {};

    ComponentShapeType shapeType;
    float radius = 0.5f;                       // sphere, capsule, cylinder
    Vec3 halfExtents = Vec3{0.5f, 0.5f, 0.5f}; // box
    float halfCylinderHeight = 0.5f;           // capsule, cylinder
};

// структура для указания настроек физического компонента при создании
struct PhysicsComponentSettings
{
    PhysicsComponentSettings(Entity entityIn, ComponentShapeType shapeTypeIn)
        : entity(entityIn)
        , shapeSettings(shapeTypeIn)
    {
    }

    Entity entity;

    EActivation activate = EActivation::DontActivate;
    PhysicsComponentMotionType motionType = PhysicsComponentMotionType::Dynamic;
    AllowedDOFs allowedDOFs = AllowedDOFs::All;
    ObjectLayer layer = Layers::MOVING;
    bool isTrigger = false; // if false controls parent transform
    float gravityFactor = 1.0f;


    ShapeCreationSettings shapeSettings;
};
}; // namespace Blainn