#pragma once
#include <Windows.h>

#include "scene/Scene.h"
#include "tools/Timeline.h"

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

    static Scene &GetActiveScene()
    {
        return m_ActiveScene;
    }

private:
    static eastl::shared_ptr<vgjs::JobSystem> m_JobSystemPtr;
    inline static Scene m_ActiveScene; // TODO: make shared?
};
} // namespace Blainn