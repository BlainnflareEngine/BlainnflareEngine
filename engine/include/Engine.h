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

    static Path &GetContentDirectory();
    static void SetContentDirectory(const Path &contentDirectory);

    static eastl::shared_ptr<Scene> GetActiveScene();
    static void SetCurrentScene(const eastl::shared_ptr<Scene> &scene);

private:
    static eastl::shared_ptr<vgjs::JobSystem> s_JobSystemPtr;
    static eastl::shared_ptr<Scene> s_ActiveScene;
    inline static Path s_contentDirectory;
};
} // namespace Blainn