#pragma once

// TODO: to pch
#include <concurrentqueue.h>
#include <entt/entt.hpp>
#include <eventpp/eventqueue.h>

#include "aliases.h"

#include "Entity.h"
#include "ImportAssetData.h"
#include "SceneEvent.h"
#include "TransformComponent.h"

#include "random.h"
#include "RenderSubsystem.h"
#include "Render/EditorCamera.h"

namespace Blainn
{
class MeshComponent;
class SceneManager;
using EntityMap = eastl::unordered_map<uuid, Entity>;

class Scene
{
public:
    Scene(const eastl::string_view &name = "UntitledScene", uuid uid = Rand::getRandomUUID(),
          bool isEditorScene = false) noexcept;
    Scene(const YAML::Node &config);
    ~Scene();

    void StartPlayMode()
    {
        m_bPlayMode = true;
    }
    void EndPlayMode()
    {
        m_bPlayMode = false;
    }

    // I'm not sure we need to copy or move scenes so if needed add these functions
    Scene(Scene &other) = delete;
    Scene &operator=(Scene &other) = delete;
    Scene(Scene &&other) = delete;
    Scene &operator=(Scene &&other) = delete;

    void Update();

    uint32_t GetViewportWidth() const
    {
        return m_ViewportWidth;
    }
    uint32_t GetViewportHeight() const
    {
        return m_ViewportHeight;
    }

    void SaveScene();
    void RestoreScene();

    eastl::string GetName() const;

    using EventHandle =
        eventpp::internal_::CallbackListBase<void(const eastl::shared_ptr<SceneEvent> &), SceneEventPolicy>::Handle;
    static EventHandle AddEventListener(const SceneEventType eventType,
                                        eastl::function<void(const SceneEventPointer &)> listener);
    static void RemoveEventListener(const SceneEventType eventType, const EventHandle &handle);

    Entity CreateEntity(const eastl::string &name = "", bool onSceneChanged = false, bool createdByEditor = false);
    Entity CreateChildEntity(Entity parent, const eastl::string &name = "", bool onSceneChanged = false,
                             bool createdByEditor = false);
    Entity CreateEntityWithID(const uuid &id, const eastl::string &name = "", bool shouldSort = true,
                              bool onSceneChanged = false, bool createdByEditor = false);
    Entity CreateChildEntityWithID(Entity parent, const uuid &id, const eastl::string &name = "",
                                   bool shouldSort = true, bool onSceneChanged = false, bool createdByEditor = false);
    void CreateEntities(const YAML::Node &entitiesNode, bool onSceneChanged = false, bool createdByEditor = false);
    void LoadNavMeshData(const YAML::Node &node);
    void SubmitToDestroyEntity(Entity entity);


    Entity GetEntityWithUUID(const uuid &id) const;
    Entity TryGetEntityWithUUID(const uuid &id) const;
    Entity TryGetEntityWithTag(const eastl::string &tag);
    Entity TryGetDescendantEntityWithTag(Entity entity, const eastl::string &tag) const;

    void GetEntitiesInHierarchy(eastl::vector<Entity> &outEntities);

    void CreateAttachMeshComponent(Entity entity, const Path &path, const ImportMeshData &data);

    template <typename... Components> auto GetAllEntitiesWith()
    {
        return m_Registry.view<Components...>();
    }

    void ParentEntity(Entity entity, Entity parent);
    void UnparentEntity(Entity entity, bool convertToWorldSpace = true);

    Entity DuplicateEntity(Entity entity);

    void ConvertToLocalSpace(Entity entity);
    void ConvertToWorldSpace(Entity entity);
    Mat4 GetWorldSpaceTransformMatrix(Entity entity);
    void SetFromWorldSpaceTransformMatrix(Entity entity, Mat4 worldTransform);
    TransformComponent GetWorldSpaceTransform(Entity entity);

    uuid &GetSceneID()
    {
        return m_SceneID;
    }
    // prefabs would be cool
    // Entity CreatePrefabEntity(Entity entity, Entity parent /* and so on */);

private:
    void DestroyEntityInternal(Entity entity, bool excludeChildren = false, bool first = true);
    void DestroyEntityInternal(const uuid &entityID, bool excludeChildren = false, bool first = true);

    void SortEntities();

    template <typename Fn> void SubmitPostUpdateFunc(Fn &&func)
    {
        s_postUpdateQueue.enqueue(func);
    }

    void ReportEntityReparent(Entity entity);

    void ProcessEvents();

    static void ProcessStaticEvents();

private:
    uuid m_SceneID;
    entt::registry m_Registry;
    eastl::string m_Name;
    bool m_IsEditorScene{false};
    uint32_t m_ViewportWidth{0}, m_ViewportHeight{0};

    // TODO: remove this trash
    EntityMap m_EntityIdMap;

    moodycamel::ConcurrentQueue<eastl::function<void()>> s_postUpdateQueue;

    inline static eventpp::EventQueue<SceneEventType, void(const SceneEventPointer &), SceneEventPolicy>
        s_sceneEventQueue;

    bool m_bPlayMode{false};

    eastl::shared_ptr<Camera> m_editorCam;

    friend class Entity;
    friend class SceneManager;
};
} // namespace Blainn

#include "EntityTemplates.h"