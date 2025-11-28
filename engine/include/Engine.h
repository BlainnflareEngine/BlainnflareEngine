#pragma once

#include <Windows.h>
#include "Render/Device.h"
#include "Scene/Scene.h"
#include <functional>

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
        static void Init();
        static void InitRenderSubsystem(HWND windowHandle);
        static void Destroy();
        static void Update(float deltaTime);

    public:
        static Path &GetContentDirectory();
        static void SetContentDirectory(const Path &contentDirectory);
        static void SetDefaultContentDirectory();

    public:
        static eastl::shared_ptr<Scene> GetActiveScene();
        static void SetActiveScene(const eastl::shared_ptr<Scene> &scene);
        static void ClearActiveScene();

    public:
        static eastl::shared_ptr<Device> GetGraphicsDevice();

        static HWND CreateBlainnWindow(UINT width, UINT height, const std::string &winTitle,
                                       const std::string &winClassTitle, HINSTANCE hInst);
        static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    private:
        static inline eastl::function<void(float)> m_renderFunc = nullptr;
        static inline eastl::shared_ptr<vgjs::JobSystem> s_JobSystemPtr = nullptr;
        static inline eastl::shared_ptr<Scene> s_activeScene{};
        static inline Path s_contentDirectory;
        static eastl::shared_ptr<Device> m_device;
    };
} // namespace Blainn