#pragma once
#include "Subsystem.h"

namespace Blainn
{
	// RenderSubsystem
    class Freya : public Subsystem
    {
    public:
        static Freya& GetInstance();
        // TODO: virtual ~RenderSubsystem() override = default;
        virtual void Init() override;
        virtual void Destroy() override;

        void Render();
        // TODO: Additional rendering-specific methods can be added here    
    private:
        Freya(); 
        Freya(const Freya&) = delete;
        Freya& operator=(const Freya&) = delete; 
		Freya(const Freya&&) = delete;
        Freya& operator=(const Freya&&) = delete; 

		static bool m_isInitialized;
    };
} // namespace Blainn