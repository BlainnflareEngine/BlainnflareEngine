//
// Created by gorev on 11.11.2025.
//

#pragma once
#include "Entity.h"


namespace Blainn
{
class Scene;

enum class SceneEventType
{
    EntityCreated,
    EntityDestroyed,
    EntityChanged,
    EntityReparented,
    SceneChanged,
};

class SceneEvent
{
public:
    virtual ~SceneEvent() {};
    virtual SceneEventType GetEventType() = 0;
};

class SceneChangedEvent : public SceneEvent
{
public:
    SceneChangedEvent(const eastl::string &name);

    ~SceneChangedEvent() override {};

    SceneEventType GetEventType() override;

    [[nodiscard]] eastl::string &GetName();

private:
    eastl::string m_name;
};

class EntityEvent : public SceneEvent
{
public:
    EntityEvent(const Entity &entity, const uuid &id, bool sceneChanged);

    ~EntityEvent() override {};

    [[nodiscard]] Entity GetEntity() const;
    [[nodiscard]] uuid GetUUID() const;
    [[nodiscard]] bool IsSceneChanged() const;

protected:
    Entity m_entity;
    uuid m_uuid;
    bool m_isSceneChanged = false;
};

class EntityCreatedEvent : public EntityEvent
{
public:
    EntityCreatedEvent(const Entity &entity, const uuid &id, bool sceneChanged = false, bool createdByEditor = false);

    ~EntityCreatedEvent() override {};

    SceneEventType GetEventType() override;

    [[nodiscard]] Entity GetParent() const;
    [[nodiscard]] bool CreatedByEditor() const;

private:
    bool m_createdByEditor = false;
};

class EntityDestroyedEvent : public EntityEvent
{
public:
    EntityDestroyedEvent(const Entity &entity, const uuid &id, bool sceneChanged = false);

    ~EntityDestroyedEvent() override {};

    SceneEventType GetEventType() override;
};

class EntityChangedEvent : public EntityEvent
{
public:
    EntityChangedEvent(const Entity &entity, const uuid &id, bool sceneChanged = false);

    ~EntityChangedEvent() override {};

    SceneEventType GetEventType() override;
};

class EntityReparentedEvent : public EntityChangedEvent
{
public:
    EntityReparentedEvent(const Entity &entity, const uuid &id);
    ~EntityReparentedEvent() override {};
    SceneEventType GetEventType() override;
};

using SceneEventPointer = eastl::shared_ptr<SceneEvent>;

struct SceneEventPolicy
{
    static SceneEventType getEvent(const SceneEventPointer &sceneEvent);
};

} // namespace Blainn
