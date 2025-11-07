#pragma once

#include <Windows.h>
#include "scene/Scene.h"

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
    static HWND CreateBlainnWindow(UINT width, UINT height, const std::string &winTitle, const std::string &winClassTitle, HINSTANCE hInst);
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    static eastl::shared_ptr<vgjs::JobSystem> m_JobSystemPtr;
    static Scene m_ActiveScene; // TODO: make shared?
};
} // namespace Blainn