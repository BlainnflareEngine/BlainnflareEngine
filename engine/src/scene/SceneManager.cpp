//
// Created by gorev on 30.01.2026.
//

#include "../../include/scene/SceneManager.h"

#include "Engine.h"
#include "scene/Scene.h"

#include <map>

namespace Blainn
{

void SceneManager::CloseScenes()
{
    if (m_activeScene) CloseScene(m_activeScene->GetSceneID());
}


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
    if (m_activeScene && m_activeScene->GetSceneID() == id) return m_activeScene;

    if (m_additiveScenes.contains(id)) return m_additiveScenes[id];


    BF_WARN("Not found scene with ID {}", id.str());
    return nullptr;
}


eastl::shared_ptr<Scene> SceneManager::OpenScene(const YAML::Node &config, SceneLoadType loadType)
{
    auto scenePtr = eastl::make_shared<Scene>(config);
    HandleLoadType(loadType, scenePtr);
    RebuildAllScenesList();
    return scenePtr;
}


void SceneManager::HandleLoadType(SceneLoadType loadType, eastl::shared_ptr<Scene> scenePtr)
{
    switch (loadType)
    {
    case Single:
        m_activeScene = scenePtr;
        // TODO: load additive scenes from main scene
        m_additiveScenes.clear();
        break;
    case Additive:
        m_additiveScenes[scenePtr->GetSceneID()] = scenePtr;
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
        RebuildAllScenesList();
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
    if (m_activeScene && m_activeScene->GetName() == sceneName)
    {
        CloseScene(m_activeScene->GetSceneID());
        return;
    }

    for (auto &[id, scene] : m_additiveScenes)
    {
        if (scene->GetName() == sceneName)
        {
            CloseScene(scene->GetSceneID());
            return;
        }
    }

    BF_WARN("Not found scene with name {}", sceneName.c_str());
}


void SceneManager::CloseScene(const uuid &id)
{
    if (m_activeScene && m_activeScene->GetSceneID() == id)
    {
        m_activeScene.reset();
        // SetActiveScene(m_additiveScenes.begin()->first);
        m_additiveScenes.clear();
        m_allScenes.clear();
        return;
    }

    if (m_additiveScenes.contains(id))
    {
        m_additiveScenes.erase(id);
        RebuildAllScenesList();
        return;
    }

    BF_WARN("Not found scene with name {}", id.str());
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


eastl::vector<eastl::shared_ptr<Scene>> &SceneManager::GetActiveScenes()
{
    return m_allScenes;
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
            SetActiveScene(scene->GetSceneID());
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

    if (m_activeScene) m_additiveScenes[m_activeScene->GetSceneID()] = m_activeScene;

    m_activeScene = m_additiveScenes[id];
    m_additiveScenes.erase(id);
    RebuildAllScenesList();
}

/**
 * Sets main scene from additive scenes pool
 */
void SceneManager::SetActiveScene(const eastl::shared_ptr<Scene> &scene)
{
    SetActiveScene(scene->GetSceneID());
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


Entity SceneManager::TryGetEntityWithUUID(const uuid &id) const
{
    BLAINN_PROFILE_FUNC();

    Entity entity = Entity();

    if (m_activeScene) entity = m_activeScene->TryGetEntityWithUUID(id);

    if (entity.IsValid()) return entity;

    for (auto &[uuid, scene] : m_additiveScenes)
    {
        entity = scene->TryGetEntityWithUUID(id);
        if (entity.IsValid()) return entity;
    }

    return entity;
}


Entity SceneManager::TryGetEntityWithTag(const eastl::string &tag)
{
    BLAINN_PROFILE_FUNC();

    Entity entity = Entity();

    if (m_activeScene) entity = m_activeScene->TryGetEntityWithTag(tag);

    if (entity.IsValid()) return entity;

    for (auto &[id, scene] : m_additiveScenes)
    {
        entity = scene->TryGetEntityWithTag(tag);
        if (entity.IsValid()) return entity;
    }

    return entity;
}


Entity SceneManager::TryGetDescendantEntityWithTag(Entity entity, const eastl::string &tag) const
{
    BLAINN_PROFILE_FUNC();

    Entity e = Entity();

    if (m_activeScene) e = m_activeScene->TryGetDescendantEntityWithTag(entity, tag);

    if (e.IsValid()) return e;

    for (auto &[id, scene] : m_additiveScenes)
    {
        e = scene->TryGetDescendantEntityWithTag(entity, tag);
        if (e.IsValid()) return e;
    }

    return e;
}


void SceneManager::ParentEntity(Entity entity, Entity parent)
{
    GetScene(entity.GetSceneUUID())->ParentEntity(entity, parent);
}


void SceneManager::UnparentEntity(Entity entity, bool convertToWorldSpace)
{
    GetScene(entity.GetSceneUUID())->UnparentEntity(entity, convertToWorldSpace);
}


Entity SceneManager::DuplicateEntity(Entity entity)
{
    return GetScene(entity.GetSceneUUID())->DuplicateEntity(entity);
}


Entity SceneManager::CreateEntity(const eastl::string &name, bool onSceneChanged, bool createdByEditor)
{
    BLAINN_PROFILE_FUNC();
    return GetActiveScene()->CreateEntity(name, onSceneChanged, createdByEditor);
}


Entity SceneManager::CreateChildEntity(Entity parent, const eastl::string &name, bool onSceneChanged,
                                       bool createdByEditor)
{
    BLAINN_PROFILE_FUNC();
    return GetScene(parent.GetSceneUUID())->CreateChildEntity(parent, name, onSceneChanged, createdByEditor);
}


Entity SceneManager::CreateEntityWithID(const uuid &id, const eastl::string &name, bool shouldSort, bool onSceneChanged,
                                        bool createdByEditor)
{
    return GetActiveScene()->CreateEntityWithID(id, name, shouldSort, onSceneChanged, createdByEditor);
}


Entity SceneManager::CreateChildEntityWithID(Entity parent, const uuid &id, const eastl::string &name, bool shouldSort,
                                             bool onSceneChanged, bool createdByEditor)
{
    return GetScene(parent.GetSceneUUID())
        ->CreateChildEntityWithID(parent, id, name, shouldSort, onSceneChanged, createdByEditor);
}


void SceneManager::CreateEntities(const YAML::Node &entitiesNode, bool onSceneChanged, bool createdByEditor)
{
    GetActiveScene()->CreateEntities(entitiesNode, onSceneChanged, createdByEditor);
}


void SceneManager::SubmitToDestroyEntity(Entity entity)
{
    GetScene(entity.GetSceneUUID())->SubmitToDestroyEntity(entity);
}


Mat4 SceneManager::GetWorldSpaceTransformMatrix(Entity entity)
{
    return GetScene(entity.GetSceneUUID())->GetWorldSpaceTransformMatrix(entity);
}


TransformComponent SceneManager::GetWorldSpaceTransform(Entity entity)
{
    return GetScene(entity.GetSceneUUID())->GetWorldSpaceTransform(entity);
}


void SceneManager::SetFromWorldSpaceTransformMatrix(Entity entity, Mat4 worldTransform)
{
    return GetScene(entity.GetSceneUUID())->SetFromWorldSpaceTransformMatrix(entity, worldTransform);
}


void SceneManager::ConvertToLocalSpace(Entity entity)
{
    GetScene(entity.GetSceneUUID())->ConvertToLocalSpace(entity);
}


void SceneManager::ConvertToWorldSpace(Entity entity)
{
    GetScene(entity.GetSceneUUID())->ConvertToWorldSpace(entity);
}


void SceneManager::RebuildAllScenesList()
{
    m_allScenes.clear();
    m_allScenes.reserve(m_additiveScenes.size() + 1);

    if (m_activeScene)
    {
        m_allScenes.push_back(m_activeScene);
    }

    for (const auto &[id, scene] : m_additiveScenes)
    {
        if (scene)
        {
            m_allScenes.push_back(scene);
        }
    }
}
} // namespace Blainn