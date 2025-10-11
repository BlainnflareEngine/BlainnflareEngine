#include "pch.h"
#include "runtime/physics/PhysicsComponent.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/EActivation.h"

#include "runtime/physics/BodyBuilder.h"
#include "runtime/physics/PhysicsComponent.h"

using namespace Blainn;

PhysicsComponent::PhysicsComponent(const uuid& parentId, const uuid& componentId, BodyBuilder &builder)
{
    m_parentId = parentId;
    m_id = componentId;

    m_bodyId = builder.Build();
}

const uuid& PhysicsComponent::GetId() const
{
    return m_id;
}