//
// Created by gorev on 11.11.2025.
//
#include "scene/SceneEvent.h"

#include "scene/Entity.h"


Blainn::SceneChangedEvent::SceneChangedEvent(const eastl::shared_ptr<Scene> &scene)
    : m_scene(scene)
{
}


Blainn::SceneEventType Blainn::SceneChangedEvent::GetEventType()
{
    return SceneEventType::SceneChanged;
}


eastl::shared_ptr<Blainn::Scene> Blainn::SceneChangedEvent::GetScene() const
{
    return m_scene;
}


Blainn::EntityEvent::EntityEvent(const Entity &entity)
    : m_entity(entity)
{
}


Blainn::Entity Blainn::EntityEvent::GetEntity() const
{
    return m_entity;
}


Blainn::EntityCreatedEvent::EntityCreatedEvent(const Entity &entity)
    : EntityEvent(entity)
{
}


Blainn::SceneEventType Blainn::EntityCreatedEvent::GetEventType()
{
    return SceneEventType::EntityCreated;
}


Blainn::EntityDestroyedEvent::EntityDestroyedEvent(const Entity &entity)
    : EntityEvent(entity)
{
}


Blainn::SceneEventType Blainn::EntityDestroyedEvent::GetEventType()
{
    return SceneEventType::EntityDestroyed;
}


Blainn::SceneEventType Blainn::SceneEventPolicy::getEvent(const SceneEventPointer &sceneEvent)
{
    return sceneEvent->GetEventType();
}