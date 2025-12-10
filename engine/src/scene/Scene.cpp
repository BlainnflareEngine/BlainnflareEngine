//
//
//
#include <fstream>

#include "scene/EntityTemplates.h"
#include "scene/Scene.h"

#include "EASTL/unordered_set.h"
#include "Engine.h"
#include "ScriptingSubsystem.h"
#include "Serializer.h"
#include "ozz/base/containers/string.h"
#include "scene/SceneParser.h"

#include "sol/types.hpp"
#include "tools/Profiler.h"
#include "tools/random.h"

#include "components/MeshComponent.h"
#include "components/RenderComponent.h"
#include "subsystems/AssetManager.h"
#include "subsystems/RenderSubsystem.h"

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
    m_SceneID.fromStr(config["SceneID"].as<std::string>().c_str());

    s_sceneEventQueue.enqueue(eastl::make_shared<SceneChangedEvent>(m_Name));

    if (config["Entities"] && config["Entities"].IsSequence()) CreateEntities(config["Entities"], true);
}


Scene::~Scene()
{
    eastl::function<void()> fn;

    for (auto entity : m_EntityIdMap)
    {
        SubmitToDestroyEntity(entity.second);
    }

    while (s_postUpdateQueue.try_dequeue(fn))
    {
        BF_DEBUG("Destroying entity!");
        fn();
    }

    s_sceneEventQueue.process();
    s_sceneEventQueue.clearEvents();
}


void Scene::SaveScene()
{
    if (Engine::IsPlayMode()) return;

    BF_DEBUG("Saved scene {}", m_Name.c_str());

    YAML::Emitter out;
    out << YAML::BeginMap; // Root

    out << YAML::Key << "SceneName" << YAML::Value << m_Name.c_str();
    out << YAML::Key << "SceneID" << YAML::Value << m_SceneID.str();

    out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq; // Entities

    auto view = m_Registry.view<IDComponent>();

    for (auto it = view.rbegin(); it != view.rend(); ++it)
    {
        Entity e = {*it, this};
        if (!e) continue;

        out << YAML::BeginMap; // begin for every entity

        Serializer::Default(e, out);
        Serializer::Tag(e, out);
        Serializer::Transform(e, out);
        Serializer::Relationship(e, out);
        Serializer::Scripting(e, out);
        Serializer::Mesh(e, out);

        out << YAML::EndMap; // end for every entity
    }

    out << YAML::EndSeq; // Entities
    out << YAML::EndMap; // Root

    std::string filepath = (Engine::GetContentDirectory() / std::string(m_Name.c_str())).string();
    std::ofstream fout(filepath);
    fout << out.c_str();
}


void Scene::RestoreScene()
{
    AssetManager::OpenScene(m_Name.c_str());
}


eastl::string Scene::GetName() const
{
    return m_Name;
}


void Scene::ProcessEvents()
{
    eastl::function<void()> fn;
    while (s_postUpdateQueue.try_dequeue(fn))
    {
        fn();
    }

    s_sceneEventQueue.process();
}


/**
 * You should store EventHandle if you want to remove this listener later
 */
Scene::EventHandle Scene::AddEventListener(const SceneEventType eventType,
                                           eastl::function<void(const SceneEventPointer &)> listener)
{
    return s_sceneEventQueue.appendListener(eventType, listener);
}


void Scene::RemoveEventListener(const SceneEventType eventType, const EventHandle &handle)
{
    s_sceneEventQueue.removeListener(eventType, handle);
}


Entity Scene::CreateEntity(const eastl::string &name, bool onSceneChanged, bool createdByEditor)
{
    BF_DEBUG("Created entity! {0}", name.c_str());
    return CreateChildEntity({}, name, onSceneChanged, createdByEditor);
}

Entity Scene::CreateChildEntity(Entity parent, const eastl::string &name, bool onSceneChanged, bool createdByEditor)
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

    s_sceneEventQueue.enqueue(eastl::make_shared<EntityCreatedEvent>(entity, idComponent.ID, onSceneChanged, createdByEditor));

    return entity;
}

Entity Scene::CreateEntityWithID(const uuid &id, const eastl::string &name, bool shouldSort, bool onSceneChanged, bool createdByEditor)
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

    s_sceneEventQueue.enqueue(eastl::make_shared<EntityCreatedEvent>(entity, idComponent.ID, onSceneChanged, createdByEditor));

    return entity;
}


Entity Scene::CreateChildEntityWithID(Entity parent, const uuid &id, const eastl::string &name, bool shouldSort,
                                      bool onSceneChanged, bool createdByEditor)
{
    BLAINN_PROFILE_FUNC();

    auto entity = Entity{m_Registry.create(), this};
    auto &idComponent = entity.AddComponent<IDComponent>();
    idComponent.ID = id;

    if (!name.empty()) entity.AddComponent<TagComponent>(name);

    entity.AddComponent<RelationshipComponent>();

    if (parent) entity.SetParent(parent);

    m_EntityIdMap[idComponent.ID] = entity;

    SortEntities();

    s_sceneEventQueue.enqueue(eastl::make_shared<EntityCreatedEvent>(entity, idComponent.ID, onSceneChanged, createdByEditor));

    return entity;
}


void Scene::CreateEntities(const YAML::Node &entitiesNode, bool onSceneChanged, bool createdByEditor)
{
    if (!entitiesNode || !entitiesNode.IsSequence())
    {
        BF_WARN("Entities node is not a sequence or is empty");
        return;
    }

    BF_DEBUG("Loading {0} entities from YAML", entitiesNode.size());

    for (const auto &entityNode : entitiesNode)
    {
        uuid entityID = GetID(entityNode);
        eastl::string tag = GetTag(entityNode);

        Entity entity = CreateEntityWithID(entityID, tag, false, onSceneChanged);

        if (HasTransform(entityNode))
        {
            entity.AddComponent<TransformComponent>(GetTransform(entityNode["TransformComponent"]));
        }

        if (HasScripting(entityNode))
        {
            entity.AddComponent<ScriptingComponent>(GetScripting(entityNode["ScriptingComponent"]));
        }

        if (HasMesh(entityNode))
        {
            entity.AddComponent<MeshComponent>(GetMesh(entityNode["MeshComponent"]));
        }

        if (HasRelationship(entityNode))
        {
            auto component = GetRelationship(entityNode["RelationshipComponent"]);
            if (auto relations = entity.TryGetComponent<RelationshipComponent>())
            {
                entity.SetParentUUID(component.ParentHandle);
                relations->Children = component.Children;
            }
            else
            {
                entity.AddComponent<RelationshipComponent>(component);
            }
        }
    }
}


void Scene::SubmitToDestroyEntity(Entity entity)
{
    bool isValid = entity.IsValid();
    if (!isValid)
    {
        BF_WARN("Trying to destroy invalid entity! entt={0}", static_cast<uint32_t>(entity));
        return;
    }

    SubmitPostUpdateFunc([entity]() { entity.m_Scene->DestroyEntityInternal(entity); });
}

void Scene::DestroyEntityInternal(Entity entity, bool excludeChildren, bool first)
{
    BLAINN_PROFILE_FUNC();

    if (!entity) return;

    if (!excludeChildren)
    {
        // destroy each child, can't really iterate through them, so should think about it a bit
        // don't make this a foreach loop because entt will move the children
        //            vector in memory as entities/components get deleted
        for (size_t i = 0; i < entity.Children().size(); i++)
        {
            auto childId = entity.Children()[i];
            Entity child = GetEntityWithUUID(childId);
            DestroyEntityInternal(child, excludeChildren, false);
        }
    }

    const uuid id = entity.GetUUID();
    // if selected deselect

    if (first)
    {
        if (auto parent = entity.GetParent(); parent) parent.RemoveChild(entity);
    }

    // before actually destroying remove components that might require ID of the entity
    s_sceneEventQueue.enqueue(eastl::make_shared<EntityDestroyedEvent>(entity, id));

    ScriptingSubsystem::DestroyScriptingComponent(entity);
    m_Registry.destroy(entity);
    m_EntityIdMap.erase(id);

    SortEntities();
}

void Scene::DestroyEntityInternal(const uuid &entityID, bool excludeChildren, bool first)
{
    const auto it = m_EntityIdMap.find(entityID);
    if (it == m_EntityIdMap.end()) return;

    DestroyEntityInternal(it->second, excludeChildren, first);
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


void Scene::GetEntitiesInHierarchy(eastl::vector<Entity> &outEntities)
{
    outEntities.clear();
    outEntities.reserve(m_EntityIdMap.size());
    eastl::unordered_set<uuid> visited;

    eastl::function<void(Entity)> dfs = [&](Entity e)
    {
        if (!e.IsValid()) return;

        auto id = e.GetUUID();
        if (visited.contains(id)) return;

        visited.insert(id);
        outEntities.push_back(e);

        if (auto *rel = e.TryGetComponent<RelationshipComponent>())
        {
            for (const auto &childUUID : rel->Children)
            {
                auto child = GetEntityWithUUID(childUUID);
                dfs(child);
            }
        }
    };

    for (auto [entity, idComponent] : GetAllEntitiesWith<IDComponent>().each())
    {
        Entity e = GetEntityWithUUID(idComponent.ID);
        if (!e.GetParent().IsValid())
        {
            dfs(e);
        }
    }
}


void Blainn::Scene::CreateAttachMeshComponent(Entity entity, const Path &path, const ImportMeshData &data)
{
    MeshComponent *meshComponentPtr = entity.TryGetComponent<MeshComponent>();
    if (meshComponentPtr)
    {
        BF_ERROR("entity alrady has mesh component");
        return;
    }

    eastl::shared_ptr<MeshHandle> handlePtr;
    AssetManager &assetManagerInstance = AssetManager::GetInstance();
    if (assetManagerInstance.HasMesh(path))
    {
        handlePtr = assetManagerInstance.GetMesh(path);
    }
    else
    {
        handlePtr = assetManagerInstance.LoadMesh(path, data);
    }
    entity.AddComponent<MeshComponent>(eastl::move(handlePtr));

    RenderComponent *renderComponentPtr = entity.TryGetComponent<RenderComponent>();
    if (renderComponentPtr)
    {
        RenderSubsystem::GetInstance().AddMeshToRenderComponent(entity, handlePtr);
    }
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
    ReportEntityReparent(entity);
}

void Scene::UnparentEntity(Entity entity, bool convertToWorldSpace)
{
    Entity parent = TryGetEntityWithUUID(entity.GetParentUUID());
    if (!parent) return;

    auto &parentChildren = parent.Children();
    parentChildren.erase(eastl::remove(parentChildren.begin(), parentChildren.end(), entity.GetUUID()),
                         parentChildren.end());

    if (convertToWorldSpace) ConvertToWorldSpace(entity);

    entity.SetParentUUID(0);
    ReportEntityReparent(entity);
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

    return entity.Transform().GetTransform() * transform;
}

void Blainn::Scene::SetFromWorldSpaceTransformMatrix(Entity entity, Mat4 worldTransform)
{
    Entity parent = TryGetEntityWithUUID(entity.GetParentUUID());
    auto &entityTransform = entity.Transform();

    if (parent)
    {
        Mat4 parentTransform = GetWorldSpaceTransformMatrix(parent);
        Mat4 localTransform = parentTransform.Invert() * worldTransform;
        entityTransform.SetTransform(localTransform);
    }
    else
    {
        entityTransform.SetTransform(worldTransform);
    }
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
    // m_Registry.sort<IDComponent>(
    //     [&](const auto &lhs, const auto &rhs)
    //     {
    //         auto lhsEntity = m_EntityIdMap.find(lhs.ID);
    //         auto rhsEntity = m_EntityIdMap.find(rhs.ID);
    //         return static_cast<uint32_t>(lhsEntity->second) < static_cast<uint32_t>(rhsEntity->second);
    //     });
}


void Scene::ReportEntityReparent(Entity entity)
{
    s_sceneEventQueue.enqueue(eastl::make_shared<EntityReparentedEvent>(entity, entity.GetUUID()));
}