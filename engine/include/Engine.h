#pragma once
#include <memory>

namespace vgjs{
    class JobSystem;
}


namespace Blainn {
    class Engine
    {
        public:
            Engine() = delete;
            static void Init();
            static void Shutdown();
            static void Run();
        private:
            static std::shared_ptr<vgjs::JobSystem> m_jobSystemPtr;
    };
}