//
// Created by gorev on 30.01.2026.
//

#pragma once
#include "SceneEvent.h"

namespace Blainn
{
class Scene;

enum SceneLoadType
{
    Single,
    Additive,
};

class SceneManager
{
public:
    eastl::shared_ptr<Scene> GetScene(const eastl::string &sceneName);
    eastl::shared_ptr<Scene> GetScene(const uuid &id);
    void HandleLoadType(SceneLoadType loadType, eastl::shared_ptr<Scene> scenePtr);

    eastl::shared_ptr<Scene> OpenScene(const YAML::Node &config, SceneLoadType loadType = Single);
    eastl::shared_ptr<Scene> OpenScene(const Path &relativePath, SceneLoadType loadType = Single);
    // TODO: open by name, by id (make included scenes map in editor)

    void UpdateScenes();
    void ProcessLocalEvents();
    static void ProcessStaticEvents();

    void CloseScene(const eastl::string &sceneName);
    void CloseScene(const uuid &id);

    void SaveCurrentScene();
    void RestoreCurrentScene();

    eastl::shared_ptr<Scene> GetActiveScene();
    eastl::unordered_map<uuid, eastl::shared_ptr<Scene>> &GetAdditiveScenes();

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

private:
    eastl::shared_ptr<Scene> m_activeScene;
    eastl::unordered_map<uuid, eastl::shared_ptr<Scene>> m_additiveScenes = {};
    // TODO: editor scenes like prefab scenes, material scenes etc...

    bool m_bPlayMode = false;
};

} // namespace Blainn