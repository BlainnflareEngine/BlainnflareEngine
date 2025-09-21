#include "runtime/physics/PhysicsComponent.h"

#include "runtime/physics/BodyBuilder.h"

Blainn::PhysicsComponent::PhysicsComponent(uuid parentId, uuid componentId, const BodyBuilder &builder)
{
    m_parentId = parentId;
    m_id = componentId;

    JPH::Body* id = JPH::BodyInterface().CreateBody(builder.getBodySettings());
}