#pragma once

#include "EngineConfig.h"
#include "Render/Device.h"
#include "Scene/Scene.h"
#include "tools/Timeline.h"
#include <functional>

#include "SelectionManager.h"
#include "scene/SceneManager.h"

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>

namespace vgjs
{
class JobSystem;
}

namespace Blainn
{
class Engine
{
public:
    Engine() = delete;
    static void Init(Timeline<eastl::chrono::milliseconds> &globalTimeline);
    static void InitRenderSubsystem(HWND windowHandle);
    static void InitAISubsystems();
    static void Destroy();
    static void Update(float deltaTime);
    static float GetDeltaTime();

    static void StartPlayMode();
    static void TogglePausePlayMode();
    static void EscapePlayMode();
    static bool IsPlayMode();
    static bool PlayModePaused();
    static void InitScenePlayMode();
    static void InitAISubsystem();

public:
    static Path &GetContentDirectory();
    static void SetContentDirectory(const Path &contentDirectory);
    static void SetDefaultContentDirectory();

    static EngineConfig &GetConfig();

public:
    static HWND CreateBlainnWindow(UINT width, UINT height, const std::string &winTitle,
                                   const std::string &winClassTitle, HINSTANCE hInst);
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    static SelectionManager &GetSelectionManager()
    {
        return s_selectionManager;
    }

    static SceneManager &GetSceneManager()
    {
        return s_sceneManager;
    }

private:
    static inline SelectionManager s_selectionManager = SelectionManager();
    static inline eastl::function<void(float)> s_renderFunc = nullptr;
    static inline eastl::shared_ptr<vgjs::JobSystem> s_JobSystemPtr = nullptr;
    // static inline eastl::shared_ptr<Scene> s_activeScene{};
    static inline SceneManager s_sceneManager = SceneManager();
    static inline Path s_contentDirectory;
    static inline Timeline<eastl::chrono::milliseconds> s_playModeTimeline{nullptr};
    static inline bool s_isPlayMode = false;
    static inline bool s_playModePaused = false;
    static inline float s_deltaTime = 0.0f;
    static inline eastl::string s_startPlayModeSceneName;
    static inline EngineConfig s_config;
};
} // namespace Blainn
