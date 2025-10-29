#pragma once
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
    static void Init();
    static void InitRenderSubsystem(HWND windowHandle);
    static void Destroy();
    static void Update(float deltaTime);

private:
    static eastl::shared_ptr<vgjs::JobSystem> m_jobSystemPtr;
};
} // namespace Blainn