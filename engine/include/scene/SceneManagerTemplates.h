//
// Created by gorev on 02.02.2026.
//

#pragma once

#include <entt/entt.hpp>
#include "Scene.h"
#include "SceneManager.h"

#include "scene/BasicComponents.h"
#include "scene/TransformComponent.h"
#include "components/AIControllerComponent.h"
#include "components/CameraComponent.h"
#include "components/LightComponent.h"
#include "components/MeshComponent.h"
#include "components/NavMeshVolumeComponent.h"
#include "components/PerceptionComponent.h"
#include "components/PhysicsComponent.h"
#include "components/ScriptingComponent.h"
#include "components/SkyboxComponent.h"
#include "components/StimulusComponent.h"

namespace Blainn
{

template <typename... Components>
using GlobalViewList = eastl::vector<entt::basic_view<entt::entity, entt::get_t<Components...>>>;

template <typename... Components> GlobalViewList<Components...> SceneManager::GetAllEntitiesWith()
{
    GlobalViewList<Components...> views;
    views.reserve(GetAdditiveScenes().size() + 1);

    if (m_activeScene) views.emplace_back(m_activeScene->GetAllEntitiesWith<Components...>());

    for (const auto &[id, scene] : m_additiveScenes)
    {
        if (scene)
        {
            views.emplace_back(scene->GetAllEntitiesWith<Components...>());
        }
    }
    return views;
}
} // namespace Blainn
