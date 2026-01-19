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


Blainn::EntityEvent::EntityEvent(const Entity &entity, const uuid &id, bool sceneChanged)
    : m_entity(entity)
    , m_uuid(id)
    , m_isSceneChanged(sceneChanged)
{
}


Blainn::Entity Blainn::EntityEvent::GetEntity() const
{
    return m_entity;
}


Blainn::uuid Blainn::EntityEvent::GetUUID() const
{
    return m_uuid;
}


bool Blainn::EntityEvent::IsSceneChanged() const
{
    return m_isSceneChanged;
}


Blainn::EntityCreatedEvent::EntityCreatedEvent(const Entity &entity, const uuid &id, bool sceneChanged,
                                               bool createdByEditor)
    : EntityEvent(entity, id, sceneChanged)
    , m_createdByEditor(createdByEditor)
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


bool Blainn::EntityCreatedEvent::CreatedByEditor() const
{
    return m_createdByEditor;
}


Blainn::EntityDestroyedEvent::EntityDestroyedEvent(const Entity &entity, const uuid &id, bool sceneChanged)
    : EntityEvent(entity, id, sceneChanged)
{
}


Blainn::SceneEventType Blainn::EntityDestroyedEvent::GetEventType()
{
    return SceneEventType::EntityDestroyed;
}


Blainn::EntityChangedEvent::EntityChangedEvent(const Entity &entity, const uuid &id, bool sceneChanged)
    : EntityEvent(entity, id, sceneChanged)
{
}


Blainn::SceneEventType Blainn::EntityChangedEvent::GetEventType()
{
    return SceneEventType::EntityChanged;
}


Blainn::EntityReparentedEvent::EntityReparentedEvent(const Entity &entity, const uuid &id)
    : EntityChangedEvent(entity, id, false)
{
}


Blainn::SceneEventType Blainn::EntityReparentedEvent::GetEventType()
{
    return SceneEventType::EntityReparented;
}


Blainn::SceneEventType Blainn::SceneEventPolicy::getEvent(const SceneEventPointer &sceneEvent)
{
    return sceneEvent->GetEventType();
}