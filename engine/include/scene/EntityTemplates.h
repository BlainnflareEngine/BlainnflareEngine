#pragma once

namespace Blainn
{

template<typename T, typename... Args>
T& Entity::AddComponent(Args&&... args)
{
    assert(HasComponent<T>() || "Entity doesn't have the component!");
    return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
}

template<typename T>
T& Entity::GetComponent() const
{
    assert(HasComponent<T>() || "Entity doesn't have the component!");
    return m_Scene->m_Registry.get<T>(m_EntityHandle);
}

// Returns nullptr if entity does not have the requested component
template<typename T>
T* Entity::TryGetComponent()
{
    assert(IsValid());
    return m_Scene->m_Registry.try_get<T>(m_EntityHandle);
}

// Returns nullptr if entity does not have the requested component
template<typename T>
const T* Entity::TryGetComponent() const
{
    assert(IsValid());
    return m_Scene->m_Registry.try_get<T>(m_EntityHandle);
}

template<typename T>
bool Entity::HasComponent()
{
    assert(IsValid());
    return m_Scene->m_Registry.all_of<T>(m_EntityHandle);
}
template<typename T>
bool Entity::HasComponent() const
{
    assert(IsValid());
    return m_Scene->m_Registry.all_of<T>(m_EntityHandle);
}

template<typename... T>
bool Entity::HasAny()
{
    assert(IsValid());
    return m_Scene->m_Registry.any_of<T...>(m_EntityHandle);
}
template<typename... T>
bool Entity::HasAny() const
{
    assert(IsValid());
    return m_Scene->m_Registry.any_of<T...>(m_EntityHandle);
}

template<typename... T>
bool Entity::HasAll()
{
    assert(IsValid());
    return m_Scene->m_Registry.all_of<T...>(m_EntityHandle);
}
template<typename... T>
bool Entity::HasAll() const
{
    assert(IsValid());
    return m_Scene->m_Registry.all_of<T...>(m_EntityHandle);
}

template<typename T>
void Entity::RemoveComponent()
{
    assert(HasComponent<T>() || "Entity doesn't have the component!");
    return m_Scene->m_Registry.erase<T>(m_EntityHandle);
}
template<typename T>
void Entity::RemoveComponentIfExists()
{
    assert(IsValid());
    return m_Scene->m_Registry.remove<T>(m_EntityHandle);
}

}