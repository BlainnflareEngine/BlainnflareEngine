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

private:
    static eastl::shared_ptr<vgjs::JobSystem> m_JobSystemPtr;
    static Scene m_ActiveScene; // TODO: make shared?
};
} // namespace Blainn