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
    SceneChanged,
    // TODO: reparent mb?
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
    SceneChangedEvent(const eastl::shared_ptr<Scene> &scene);

    ~SceneChangedEvent() override {};

    SceneEventType GetEventType() override;

    [[nodiscard]] eastl::shared_ptr<Scene> GetScene() const;

private:
    eastl::shared_ptr<Scene> m_scene;
};

class EntityEvent : public SceneEvent
{
public:
    EntityEvent(const Entity &entity, bool sceneChanged = false);

    ~EntityEvent() override {};

    [[nodiscard]] Entity GetEntity() const;
    [[nodiscard]] bool IsSceneChanged() const;

protected:
    Entity m_entity;
    bool m_isSceneChanged = false;
};

class EntityCreatedEvent : public EntityEvent
{
public:
    EntityCreatedEvent(const Entity &entity, bool sceneChanged = false);

    ~EntityCreatedEvent() override {};

    SceneEventType GetEventType() override;
};

class EntityDestroyedEvent : public EntityEvent
{
public:
    EntityDestroyedEvent(const Entity &entity, bool sceneChanged = false);

    ~EntityDestroyedEvent() override {};

    SceneEventType GetEventType() override;
};

using SceneEventPointer = eastl::shared_ptr<SceneEvent>;

struct SceneEventPolicy
{
    static SceneEventType getEvent(const SceneEventPointer &sceneEvent);
};

} // namespace Blainn
