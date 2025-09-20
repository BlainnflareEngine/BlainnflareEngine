#pragma once
#include "Subsystem.h"

namespace Blainn {
	// ScriptingSubsystem
	class Bifrost : public Subsystem {
	public:
        static Bifrost& GetInstance();

		void Init() override;
		void Destroy() override;
		// TODO:
    private:
        Bifrost(); 
        Bifrost(const Bifrost&) = delete;
        Bifrost& operator=(const Bifrost&) = delete; 
		Bifrost(const Bifrost&&) = delete;
        Bifrost& operator=(const Bifrost&&) = delete; 

		static bool m_isInitialized;
	};
}