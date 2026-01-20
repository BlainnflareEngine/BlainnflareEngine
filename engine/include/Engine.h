#pragma once

#include <Windows.h>
#include "Render/Device.h"
#include "Scene/Scene.h"
#include "tools/Timeline.h"
#include <functional>

#include "SelectionManager.h"

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
    static void Destroy();
    static void Update(float deltaTime);
    static float GetDeltaTime();

    static void StartPlayMode();
    static void StopPlayMode();
    static void EscapePlayMode();
    static bool IsPlayMode();


public:
    static Path &GetContentDirectory();
    static void SetContentDirectory(const Path &contentDirectory);
    static void SetDefaultContentDirectory();

public:
    static eastl::shared_ptr<Scene> GetActiveScene();
    static void SetActiveScene(const eastl::shared_ptr<Scene> &scene);
    static void ClearActiveScene();

public:
    static HWND CreateBlainnWindow(UINT width, UINT height, const std::string &winTitle,
                                   const std::string &winClassTitle, HINSTANCE hInst);
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    static SelectionManager& GetSelectionManager() {return m_selectionManager;}

private:
    static inline SelectionManager m_selectionManager = SelectionManager();
    static inline eastl::function<void(float)> m_renderFunc = nullptr;
    static inline eastl::shared_ptr<vgjs::JobSystem> s_JobSystemPtr = nullptr;
    static inline eastl::shared_ptr<Scene> s_activeScene{};
    static inline Path s_contentDirectory;
    static inline Timeline<eastl::chrono::milliseconds> s_playModeTimeline{nullptr};
    static inline bool s_isPlayMode = false;
    static inline float s_deltaTime = 0.0f;
};
} // namespace Blainn