//
// Created by gorev on 11.11.2025.
//
#include "scene/SceneEvent.h"

#include "scene/Entity.h"


Blainn::SceneChangedEvent::SceneChangedEvent(const eastl::string &name)
    : m_name(name)
{
}


Blainn::SceneEventType Blainn::SceneChangedEvent::GetEventType()
{
    return SceneEventType::SceneChanged;
}


eastl::string &Blainn::SceneChangedEvent::GetName()
{
    return m_name;
}


Blainn::EntityEvent::EntityEvent(const Entity &entity, bool sceneChanged)
    : m_entity(entity)
    , m_isSceneChanged(sceneChanged)
{
}


Blainn::Entity Blainn::EntityEvent::GetEntity() const
{
    return m_entity;
}


bool Blainn::EntityEvent::IsSceneChanged() const
{
    return m_isSceneChanged;
}


Blainn::EntityCreatedEvent::EntityCreatedEvent(const Entity &entity, bool sceneChanged)
    : EntityEvent(entity, sceneChanged)
{
}


Blainn::SceneEventType Blainn::EntityCreatedEvent::GetEventType()
{
    return SceneEventType::EntityCreated;
}


Blainn::Entity Blainn::EntityCreatedEvent::GetParent() const
{
    return m_entity.GetParent();
}


Blainn::EntityDestroyedEvent::EntityDestroyedEvent(const Entity &entity, bool sceneChanged)
    : EntityEvent(entity, sceneChanged)
{
}


Blainn::SceneEventType Blainn::EntityDestroyedEvent::GetEventType()
{
    return SceneEventType::EntityDestroyed;
}


Blainn::EntityChangedEvent::EntityChangedEvent(const Entity &entity, bool sceneChanged)
    : EntityEvent(entity, sceneChanged)
{
}


Blainn::SceneEventType Blainn::EntityChangedEvent::GetEventType()
{
    return SceneEventType::EntityChanged;
}


Blainn::SceneEventType Blainn::SceneEventPolicy::getEvent(const SceneEventPointer &sceneEvent)
{
    return sceneEvent->GetEventType();
}