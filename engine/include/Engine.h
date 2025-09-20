#pragma once

namespace Blainn {
    class Engine
    {
        public:
        Engine() = delete;
        static void Init();
        static void Shutdown();
        static void Run();
    };
}