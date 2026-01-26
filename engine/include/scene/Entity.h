//
// Created by admin on 13-Oct-25.
//
#pragma once

#include "BasicComponents.h"
#include "TransformComponent.h"
#include "aliases.h"

namespace Blainn
{
struct TransformComponent;
class Scene;

class Entity
{
public:
    Entity() = default;
    Entity(entt::entity handle, Scene *scene)
        : m_EntityHandle(handle)
        , m_Scene(scene)
    {
    }
    ~Entity() = default;

#pragma region conversion and comparison operators

    operator uint32_t() const
    {
        return static_cast<uint32_t>(m_EntityHandle);
    }
    operator entt::entity() const
    {
        return m_EntityHandle;
    }
    operator bool() const;

    bool operator==(const Entity &other) const
    {
        return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
    }

    bool operator!=(const Entity &other) const
    {
        return !(*this == other);
    }

#pragma endregion

    bool IsValid() const;

    eastl::string &Name()
    {
        return HasComponent<TagComponent>() ? GetComponent<TagComponent>().Tag : s_NoName;
    }
    const eastl::string &Name() const
    {
        return HasComponent<TagComponent>() ? GetComponent<TagComponent>().Tag : s_NoName;
    }

    uuid GetUUID() const
    {
        return GetComponent<IDComponent>().ID;
    }
    uuid GetSceneUUID() const;

    TransformComponent& Transform() { return GetComponent<TransformComponent>(); }
    const Mat4& Transform() const { return GetComponent<TransformComponent>().GetTransform(); }

#pragma region Utility templates for queriyng and managing components

    template <typename T, typename... Args> T &AddComponent(Args &&...args);

    template <typename T> T &GetComponent() const;

    // Returns nullptr if entity does not have the requested component
    template <typename T> T *TryGetComponent();

    // Returns nullptr if entity does not have the requested component
    template <typename T> const T *TryGetComponent() const;

    template <typename T> bool HasComponent();
    template <typename T> bool HasComponent() const;

    bool HasComponent(entt::id_type typeId);

    template <typename... T> bool HasAny();
    template <typename... T> bool HasAny() const;

    template <typename... T> bool HasAll();
    template <typename... T> bool HasAll() const;

    template <typename T> void RemoveComponent();
    template <typename T> bool RemoveComponentIfExists();

#pragma endregion

#pragma region Hierarchy functions

    Entity GetParent() const;
    void SetParent(Entity parent);

    uuid GetParentUUID() const
    {
        return GetComponent<RelationshipComponent>().ParentHandle;
    }
    void SetParentUUID(const uuid &parent)
    {
        GetComponent<RelationshipComponent>().ParentHandle = parent;
    }

    eastl::vector<uuid> &Children()
    {
        return GetComponent<RelationshipComponent>().Children;
    }
    const eastl::vector<uuid> &Children() const
    {
        return GetComponent<RelationshipComponent>().Children;
    }

    bool RemoveChild(Entity child);

    bool IsAncestorOf(Entity entity);
    bool IsDescendantOf(Entity entity) const
    {
        return entity.IsAncestorOf(*this);
    }

#pragma endregion

private:
    entt::entity m_EntityHandle = entt::null;
    Scene *m_Scene;

    inline static eastl::string s_NoName{"unnamed"};

    friend class Scene;
};
} // namespace Blainn
