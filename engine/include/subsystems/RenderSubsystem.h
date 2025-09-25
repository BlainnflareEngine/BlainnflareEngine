#pragma once

namespace Blainn
{
    class RenderSubsystem
    {
    public:
        // TODO: virtual ~RenderSubsystem() override = default;
        static void Init();
        static void Destroy();

        static void Render();
        // TODO: Additional rendering-specific methods can be added here    
    private:
        RenderSubsystem() = delete; 
        RenderSubsystem(const RenderSubsystem&) = delete;
        RenderSubsystem& operator=(const RenderSubsystem&) = delete; 
		RenderSubsystem(const RenderSubsystem&&) = delete;
        RenderSubsystem& operator=(const RenderSubsystem&&) = delete; 

		static bool m_isInitialized;
    };
} // namespace Blainn