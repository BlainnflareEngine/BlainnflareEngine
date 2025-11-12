//
//
//

#pragma once
#include "EASTL/string.h"
#include "EASTL/string_view.h"
#include "EASTL/unordered_map.h"
#include "Entity.h"
#include "SceneEvent.h"
#include "TransformComponent.h"
#include "aliases.h"
#include "concurrentqueue.h"
#include "entt/entt.hpp"
#include "eventpp/eventqueue.h"
#include "random.h"

namespace Blainn
{

using EntityMap = eastl::unordered_map<uuid, Entity>;
class Scene
{
public:
    Scene(const eastl::string_view &name = "UntitledScene", uuid uid = Rand::getRandomUUID(),
          bool isEditorScene = false) noexcept;
    Scene(const YAML::Node &config);
    ~Scene();
    
    // I'm not sure we need to copy or move scenes so if needed add these functions
    Scene(Scene &other) = delete;
    Scene &operator=(Scene &other) = delete;
    Scene(Scene &&other) = delete;
    Scene &operator=(Scene &&other) = delete;

    // TODO: some deltatime should be passed to it, I'm confused about our times
    void UpdateRuntime(/*todo float deltatime*/);
    void UpdateEditor(/*todo float deltatime*/);

    void OnRenderRuntime(/*todo Renderer& renderer, float deltatime*/);
    void OnRenderEditor(/*todo Renderer& renderer, float deltatime, EditorCamera& editorCamera*/);

    void OnRuntimeStart();
    void OnRuntimeStop();

    void SetViewportSize(uint32_t width, uint32_t height);
    uint32_t GetViewportWidth() const
    {
        return m_ViewportWidth;
    }
    uint32_t GetViewportHeight() const
    {
        return m_ViewportHeight;
    }

    void SaveScene();

    static void ProcessEvents();
    using EventHandle =
        eventpp::internal_::CallbackListBase<void(const eastl::shared_ptr<SceneEvent> &), SceneEventPolicy>::Handle;
    static EventHandle AddEventListener(const SceneEventType eventType,
                                        eastl::function<void(const SceneEventPointer &)> listener);
    static void RemoveEventListener(const SceneEventType eventType, const EventHandle &handle);

    Entity CreateEntity(const eastl::string &name = "");
    Entity CreateChildEntity(Entity parent, const eastl::string &name = "");
    Entity CreateEntityWithID(const uuid &id, const eastl::string &name = "", bool shouldSort = true);
    void SubmitToDestroyEntity(Entity entity);
    void DestroyEntity(Entity entity, bool excludeChildren = false, bool first = true);
    void DestroyEntity(const uuid &entityID, bool excludeChildren = false, bool first = true);

    Entity GetEntityWithUUID(const uuid &id) const;
    Entity TryGetEntityWithUUID(const uuid &id) const;
    Entity TryGetEntityWithTag(const eastl::string &tag);
    Entity TryGetDescendantEntityWithTag(Entity entity, const eastl::string &tag) const;

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
    TransformComponent GetWorldSpaceTransform(Entity entity);

    // prefabs would be cool
    // Entity CreatePrefabEntity(Entity entity, Entity parent /* and so on */);

private:
    void SortEntities();

    template <typename Fn> void SubmitPostUpdateFunc(Fn &&func)
    {
        s_postUpdateQueue.enqueue(func);
    }

private:
    uuid m_SceneID;
    entt::entity m_SceneEntity{entt::null};
    entt::registry m_Registry;

    eastl::string m_Name;
    bool m_IsEditorScene{false};
    uint32_t m_ViewportWidth{0}, m_ViewportHeight{0};

    EntityMap m_EntityIdMap;

    inline static moodycamel::ConcurrentQueue<eastl::function<void()>> s_postUpdateQueue;

    inline static eventpp::EventQueue<SceneEventType, void(const SceneEventPointer &), SceneEventPolicy>
        s_sceneEventQueue;


    /*
     *      we would hold the lights here
     *      Cherno does it with LightEnvironment struct where all the lights are stored.
     *      He also separately stores the main dir light which casts the shadows.
     */

    friend class Entity;
};
} // namespace Blainn

#include "EntityTemplates.h"