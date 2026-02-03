//
// Created by gorev on 30.01.2026.
//

#pragma once
#include "SceneEvent.h"
#include <entt/entt.hpp>

namespace Blainn
{
class Scene;

enum SceneLoadType
{
    Single,
    Additive,
};

template <typename... Components>
using GlobalViewList = eastl::vector<entt::basic_view<entt::entity, entt::get_t<Components...>>>;

class SceneManager
{
public:
    eastl::shared_ptr<Scene> GetScene(const eastl::string &sceneName);
    eastl::shared_ptr<Scene> GetScene(const uuid &id);

    eastl::shared_ptr<Scene> OpenScene(const YAML::Node &config, SceneLoadType loadType = Single);
    eastl::shared_ptr<Scene> OpenScene(const Path &relativePath, SceneLoadType loadType = Single);
    // TODO: open by name, by id (make included scenes map in editor)

    void UpdateScenes();
    void ProcessLocalEvents();
    static void ProcessStaticEvents();

    void CloseScenes();
    void CloseScene(const eastl::string &sceneName);
    void CloseScene(const uuid &id);

    void SaveCurrentScene();
    void RestoreCurrentScene();

    eastl::shared_ptr<Scene> GetActiveScene();
    eastl::unordered_map<uuid, eastl::shared_ptr<Scene>> &GetAdditiveScenes();
    eastl::vector<eastl::shared_ptr<Scene>> &GetActiveScenes();

    void SetActiveScene(const eastl::string &sceneName);
    void SetActiveScene(const uuid &id);
    void SetActiveScene(const eastl::shared_ptr<Scene> &scene);

    using SceneEventHandle =
        eventpp::internal_::CallbackListBase<void(const eastl::shared_ptr<SceneEvent> &), SceneEventPolicy>::Handle;
    static SceneEventHandle AddEventListener(const SceneEventType eventType,
                                             eastl::function<void(const SceneEventPointer &)> listener);
    static void RemoveEventListener(const SceneEventType eventType, const SceneEventHandle &handle);

    void StartPlayMode()
    {
        m_bPlayMode = true;
    }

    void EndPlayMode()
    {
        m_bPlayMode = false;
    }

    Entity TryGetEntityWithUUID(const uuid &id) const;
    Entity TryGetEntityWithTag(const eastl::string &tag);
    Entity TryGetDescendantEntityWithTag(Entity entity, const eastl::string &tag) const;

    void ParentEntity(Entity entity, Entity parent);
    void UnparentEntity(Entity entity, bool convertToWorldSpace = true);

    Entity DuplicateEntity(Entity entity);

    Entity CreateEntity(const eastl::string &name = "", bool onSceneChanged = false, bool createdByEditor = false);
    Entity CreateChildEntity(Entity parent, const eastl::string &name = "", bool onSceneChanged = false,
                             bool createdByEditor = false);
    Entity CreateEntityWithID(const uuid &id, const eastl::string &name = "", bool shouldSort = true,
                              bool onSceneChanged = false, bool createdByEditor = false);
    Entity CreateChildEntityWithID(Entity parent, const uuid &id, const eastl::string &name = "",
                                   bool shouldSort = true, bool onSceneChanged = false, bool createdByEditor = false);
    void CreateEntities(const YAML::Node &entitiesNode, bool onSceneChanged = false, bool createdByEditor = false);

    void SubmitToDestroyEntity(Entity entity);

    Mat4 GetWorldSpaceTransformMatrix(Entity entity);
    TransformComponent GetWorldSpaceTransform(Entity entity);
    void SetFromWorldSpaceTransformMatrix(Entity entity, Mat4 worldTransform);

    template <typename... Components> GlobalViewList<Components...> GetAllEntitiesWith();

private:
    eastl::shared_ptr<Scene> m_activeScene;
    eastl::unordered_map<uuid, eastl::shared_ptr<Scene>> m_additiveScenes = {};
    eastl::vector<eastl::shared_ptr<Scene>> m_allScenes = {};
    // TODO: editor scenes like prefab scenes, material scenes etc...

    bool m_bPlayMode = false;

    void RebuildAllScenesList();
    void HandleLoadType(SceneLoadType loadType, eastl::shared_ptr<Scene> scenePtr);
};

} // namespace Blainn