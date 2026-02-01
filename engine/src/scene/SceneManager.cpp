//
// Created by gorev on 30.01.2026.
//

#include "../../include/scene/SceneManager.h"

#include "Engine.h"
#include "scene/Scene.h"

#include <map>

namespace Blainn
{
eastl::shared_ptr<Scene> SceneManager::GetScene(const eastl::string &sceneName)
{
    if (m_activeScene && m_activeScene->GetName() == sceneName) return m_activeScene;

    for (auto &[id, scene] : m_additiveScenes)
        if (scene->GetName() == sceneName) return scene;

    BF_WARN("Not found scene with name {}", sceneName.c_str());
    return nullptr;
}


eastl::shared_ptr<Scene> SceneManager::GetScene(const uuid &id)
{
    if (m_activeScene && m_activeScene->GetID() == id) return m_activeScene;

    if (m_additiveScenes.contains(id)) return m_additiveScenes[id];


    BF_WARN("Not found scene with ID {}", id.str());
    return nullptr;
}


void SceneManager::HandleLoadType(SceneLoadType loadType, eastl::shared_ptr<Scene> scenePtr)
{
    switch (loadType)
    {
    case Single:
        m_activeScene = scenePtr;
        m_additiveScenes.clear();
        break;
    case Additive:
        m_additiveScenes[scenePtr->GetID()] = scenePtr;
        break;
    }
}
eastl::shared_ptr<Scene> SceneManager::OpenScene(const Path &relativePath, SceneLoadType loadType)
{
    std::string absolutePath = (Engine::GetContentDirectory() / relativePath).string();
    YAML::Node scene = YAML::LoadFile(absolutePath);

    if (scene)
    {
        auto scenePtr = eastl::make_shared<Scene>(scene);
        HandleLoadType(loadType, scenePtr);
        return scenePtr;
    }

    BF_ERROR("Scene not found. Relative path: {}, Absolute path: {}.", relativePath.string(), absolutePath);
    return nullptr;
}


void SceneManager::UpdateScenes()
{
    if (m_activeScene) m_activeScene->Update();

    for (auto &[id, scene] : m_additiveScenes)
        scene->Update();
}


void SceneManager::ProcessLocalEvents()
{
    if (m_activeScene) m_activeScene->ProcessEvents();

    for (auto &[id, scene] : m_additiveScenes)
        scene->ProcessEvents();
}


void SceneManager::ProcessStaticEvents()
{
    Scene::ProcessStaticEvents();
}


void SceneManager::CloseScene(const eastl::string &sceneName)
{

}


void SceneManager::CloseScene(const uuid &id)
{
}


void SceneManager::SaveCurrentScene()
{
    if (m_activeScene) m_activeScene->SaveScene();
}


void SceneManager::RestoreCurrentScene()
{
    if (m_activeScene) m_activeScene->RestoreScene();
}


eastl::shared_ptr<Scene> SceneManager::GetActiveScene()
{
    return m_activeScene;
}


eastl::unordered_map<uuid, eastl::shared_ptr<Scene>> &SceneManager::GetAdditiveScenes()
{
    return m_additiveScenes;
}


/**
 * Sets main scene from additive scenes pool
 */
void SceneManager::SetActiveScene(const eastl::string &sceneName)
{
    for (auto &[id, scene] : m_additiveScenes)
    {
        if (scene->GetName() == sceneName)
        {
            SetActiveScene(scene->GetID());
            return;
        }
    }

    BF_WARN("Not found additive scene with name {}", sceneName.c_str());
}

/**
 * Sets main scene from additive scenes pool
 */
void SceneManager::SetActiveScene(const uuid &id)
{
    if (!m_additiveScenes.contains(id))
    {
        BF_WARN("Not found additive scene with ID {}", id.str());
        return;
    }

    if (m_activeScene) m_additiveScenes[m_activeScene->GetID()] = m_activeScene;

    m_activeScene = m_additiveScenes[id];
    m_additiveScenes.erase(id);
}

/**
 * Sets main scene from additive scenes pool
 */
void SceneManager::SetActiveScene(const eastl::shared_ptr<Scene> &scene)
{
    SetActiveScene(scene->GetID());
}


SceneManager::SceneEventHandle SceneManager::AddEventListener(const SceneEventType eventType,
                                                              eastl::function<void(const SceneEventPointer &)> listener)
{
    return Scene::AddEventListener(eventType, listener);
}


void SceneManager::RemoveEventListener(const SceneEventType eventType, const SceneEventHandle &handle)
{
    Scene::RemoveEventListener(eventType, handle);
}
} // namespace Blainn