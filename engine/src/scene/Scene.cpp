//
//
//

#include "scene/Scene.h"

#include "tools/Profiler.h"
#include "tools/random.h"

using namespace Blainn;

Entity Scene::CreateEntity(const eastl::string &name)
{
    return CreateChildEntity({}, name);
}

Entity Scene::CreateChildEntity(Entity parent, const eastl::string &name)
{
    BLAINN_PROFILE_FUNC();

    auto entity = Entity{m_Registry.create(), this};
    auto &idComponent = entity.AddComponent<IDComponent>();
    idComponent.ID = Rand::getRandomUUID();

    /* entity.AddComponent<TransformComponent>();
     */
    if (!name.empty()) entity.AddComponent<TagComponent>(name);

    entity.AddComponent<RelationshipComponent>();

    if (parent) entity.SetParent(parent);

    m_EntityIdMap[idComponent.ID] = entity;

    SortEntities();

    return entity;
}

Entity Scene::CreateEntityWithID(const uuid &id, const eastl::string &name, bool shouldSort)
{
    BLAINN_PROFILE_FUNC();

    auto entity = Entity{m_Registry.create(), this};
    auto &idComponent = entity.AddComponent<IDComponent>();
    idComponent.ID = id;

    /* entity.AddComponent<TransformComponent>();
     */
    if (!name.empty()) entity.AddComponent<TagComponent>(name);

    entity.AddComponent<RelationshipComponent>();

    m_EntityIdMap[idComponent.ID] = entity;

    if (shouldSort) SortEntities();

    return entity;
}

void Scene::SubmitToDestroyEntity(Entity entity)
{
    bool isValid = entity.IsValid();
    if (!isValid)
    {
        BF_WARN("Trying to destroy invalid entity! entt={0}", static_cast<uint32_t>(entity));
        return;
    }

    SubmitPostUpdateFunc([entity]() { entity.m_Scene->DestroyEntity(entity); });
}

void Scene::DestroyEntity(Entity entity, bool excludeChildren, bool first)
{
    BLAINN_PROFILE_FUNC();

    if (!entity) return;

    if (!excludeChildren)
    {
        // destroy each child, can't really iterate through them, so should think about it a bit
    }

    const uuid id = entity.GetUUID();
    // if selected deselect

    if (first)
    {
        if (auto parent = entity.GetParent(); parent) parent.RemoveChild(entity);
    }

    // before actually destroying remove components that might require ID of the entity

    m_Registry.destroy(entity);
    m_EntityIdMap.erase(id);

    SortEntities();
}

void Scene::DestroyEntity(const uuid &entityID, bool excludeChildren, bool first)
{
    const auto it = m_EntityIdMap.find(entityID);
    if (it == m_EntityIdMap.end()) return;

    DestroyEntity(it->second, excludeChildren, first);
}

Entity Scene::GetEntityWithUUID(const uuid &id) const
{
    assert(m_EntityIdMap.contains(id) && "Invalid entity id or it doesn't exist");
    return m_EntityIdMap.at(id);
}

Entity Scene::TryGetEntityWithUUID(const uuid &id) const
{
    if (const auto iter = m_EntityIdMap.find(id); iter != m_EntityIdMap.end()) return iter->second;
    return Entity{};
}

Entity Scene::TryGetEntityWithTag(const eastl::string &tag)
{
    auto entities = GetAllEntitiesWith<TagComponent>();
    for (const auto &ent : entities)
    {
        if (entities.get<TagComponent>(ent).Tag == tag) return Entity(ent, const_cast<Scene *>(this));
    }
    return Entity{};
}

Entity Scene::TryGetDescendantEntityWithTag(Entity entity, const eastl::string &tag) const
{
    if (entity)
    {
        if (entity.GetComponent<TagComponent>().Tag == tag) return entity;

        for (const auto childId : entity.Children())
        {
            const Entity descendant = TryGetDescendantEntityWithTag(GetEntityWithUUID(childId), tag);
            if (descendant) return descendant;
        }
    }
    return Entity{};
}

void Scene::ParentEntity(Entity entity, Entity parent)
{
    if (parent.IsDescendantOf(entity))
    {
        UnparentEntity(parent);

        Entity newParent = TryGetEntityWithUUID(entity.GetParentUUID());
        if (newParent)
        {
            UnparentEntity(entity);
            ParentEntity(parent, newParent);
        }
    }
    else
    {
        Entity previousParent = TryGetEntityWithUUID(entity.GetParentUUID());

        if (previousParent) UnparentEntity(entity);
    }

    entity.SetParentUUID(parent.GetUUID());
    parent.Children().push_back(entity.GetUUID());

    // TODO: convert to local spce
    // ConvertToLocalSpace(entity);
}

void Scene::UnparentEntity(Entity entity, bool convertToWorldSpace)
{
    Entity parent = TryGetEntityWithUUID(entity.GetParentUUID());
    if (!parent) return;

    auto &parentChildren = parent.Children();
    parentChildren.erase(std::remove(parentChildren.begin(), parentChildren.end(), entity.GetUUID()),
                         parentChildren.end());

    // TODO
    // if (convertToWorldSpace)
    //     ConvertToWorldSpace(entity);

    entity.SetParentUUID(0);
}

Entity Scene::DuplicateEntity(Entity entity)
{
    // this one is tough, and I'm not sure we need it   :-)
    return Entity{};
}

void Scene::SortEntities()
{
    m_Registry.sort<IDComponent>(
        [&](const auto &lhs, const auto &rhs)
        {
            auto lhsEntity = m_EntityIdMap.find(lhs.ID);
            auto rhsEntity = m_EntityIdMap.find(rhs.ID);
            return static_cast<uint32_t>(lhsEntity->second) < static_cast<uint32_t>(rhsEntity->second);
        });
}