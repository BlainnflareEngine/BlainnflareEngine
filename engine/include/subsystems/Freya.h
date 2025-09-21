#pragma once

namespace Blainn
{
	// RenderSubsystem
    class Freya
    {
    public:
        // TODO: virtual ~RenderSubsystem() override = default;
        static void Init();
        static void Destroy();

        static void Render();
        // TODO: Additional rendering-specific methods can be added here    
    private:
        Freya() = delete; 
        Freya(const Freya&) = delete;
        Freya& operator=(const Freya&) = delete; 
		Freya(const Freya&&) = delete;
        Freya& operator=(const Freya&&) = delete; 

		static bool m_isInitialized;
    };
} // namespace Blainn