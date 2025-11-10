//
//
//

#include "scene/Scene.h"

#include "Engine.h"
#include "Serializer.h"
#include "ozz/base/containers/string.h"

#include "sol/types.hpp"
#include "tools/Profiler.h"
#include "tools/random.h"

#include <fstream>

using namespace Blainn;


Scene::Scene(const eastl::string_view &name, uuid uid, bool isEditorScene) noexcept
    : m_SceneID(uid)
    , m_Name(name)
    , m_IsEditorScene(isEditorScene)
{
}


Scene::Scene(const YAML::Node &config)
{
    assert(config.IsDefined());

    m_Name = config["SceneName"].as<std::string>().c_str();

    BF_DEBUG(m_Name.c_str());
    // TODO: parse all entities
}


void Scene::SaveScene()
{
    BF_DEBUG("Saved scene {}", m_Name.c_str());

    YAML::Emitter out;
    out << YAML::BeginMap; // Root

    out << YAML::Key << "SceneName" << YAML::Value << m_Name.c_str();
    out << YAML::Key << "SceneID" << YAML::Value << m_SceneID.str();

    out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq; // Entities

    auto view = m_Registry.view<IDComponent>();
    for (auto entity : view)
    {
        Entity e = {entity, this};
        if (!e) continue;

        out << YAML::BeginMap; // begin for every entity

        Serializer::Default(e, out);
        Serializer::Tag(e, out);
        Serializer::Transform(e, out);
        Serializer::Relationship(e, out);

        out << YAML::EndMap; // end for every entity
    }

    out << YAML::EndSeq; // Entities
    out << YAML::EndMap; // Root

    std::string filepath = (Engine::GetContentDirectory() / std::string(m_Name.c_str())).string();
    std::ofstream fout(filepath);
    fout << out.c_str();
}


Entity Scene::CreateEntity(const eastl::string &name)
{
    BF_DEBUG("Created entity! {0}", name.c_str());
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

    ConvertToLocalSpace(entity);
}

void Scene::UnparentEntity(Entity entity, bool convertToWorldSpace)
{
    Entity parent = TryGetEntityWithUUID(entity.GetParentUUID());
    if (!parent) return;

    auto &parentChildren = parent.Children();
    parentChildren.erase(std::remove(parentChildren.begin(), parentChildren.end(), entity.GetUUID()),
                         parentChildren.end());

    if (convertToWorldSpace) ConvertToWorldSpace(entity);

    entity.SetParentUUID(0);
}

Entity Scene::DuplicateEntity(Entity entity)
{
    // this one is tough, and I'm not sure we need it   :-)
    return Entity{};
}

void Scene::ConvertToLocalSpace(Entity entity)
{
    Entity parent = TryGetEntityWithUUID(entity.GetParentUUID());

    if (!parent) return;

    auto &transform = entity.Transform();
    auto parentTransform = GetWorldSpaceTransformMatrix(parent);
    auto localTransform = parentTransform.Invert() * transform.GetTransform();
    transform.SetTransform(localTransform);
}

void Scene::ConvertToWorldSpace(Entity entity)
{
    Entity parent = TryGetEntityWithUUID(entity.GetParentUUID());

    if (!parent) return;

    Mat4 transform = GetWorldSpaceTransformMatrix(entity);
    auto &entityTransform = entity.Transform();
    entityTransform.SetTransform(transform);
}

Mat4 Scene::GetWorldSpaceTransformMatrix(Entity entity)
{
    Mat4 transform = Mat4::Identity;

    Entity parent = TryGetEntityWithUUID(entity.GetParentUUID());

    if (parent) return GetWorldSpaceTransformMatrix(parent);

    return transform * entity.Transform().GetTransform();
}

TransformComponent Scene::GetWorldSpaceTransform(Entity entity)
{
    Mat4 transform = GetWorldSpaceTransformMatrix(entity);
    TransformComponent transformComponent;
    transformComponent.SetTransform(transform);
    return transformComponent;
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