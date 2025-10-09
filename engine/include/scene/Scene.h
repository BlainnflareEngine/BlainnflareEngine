//
//
//

#pragma once
#include "aliases.h"
#include "EASTL/string.h"
#include "EASTL/string_view.h"
#include "entt/entt.hpp"


namespace Blainn
{
    class Scene
    {
    public:
        Scene(const eastl::string_view& name = "UntitledScene", bool isEditorScene = false) noexcept;
        ~Scene();
        // I'm not sure we need to copy or move scenes so if needed add these functions
        Scene(Scene&) = delete;
        Scene& operator=(Scene&) = delete;
        Scene(Scene&&) = delete;
        Scene& operator=(Scene&&) = delete;

        // TODO: some deltatime should be passed to it, I'm confused about our times
        void UpdateRuntime(/*todo float deltatime*/);
        void UpdateEditor (/*todo float deltatime*/);

        void OnRenderRuntime(/*todo Renderer& renderer, float deltatime*/);
        void OnRenderEditor (/*todo Renderer& renderer, float deltatime, EditorCamera& editorCamera*/);

        void OnRuntimeStart();
        void OnRuntimeStop();

        void SetViewportSize(uint32_t width, uint32_t height);

        entt::entity CreateEntity(const eastl::string& name = "");
    private:
        uuid m_SceneID;
        entt::entity m_SceneEntity{entt::null};
        entt::registry m_Registry;

        eastl::string m_Name;
        bool m_IsEditorScene{false};
        uint32_t m_ViewportWidth{0}, m_ViewportHeight{0};

/*
 *      we would hold the lights here
 *      Cherno does it with LightEnvironment struct where all the lights are stored.
 *      He also separately stores the main dir light which casts the shadows.
 */

    };
}