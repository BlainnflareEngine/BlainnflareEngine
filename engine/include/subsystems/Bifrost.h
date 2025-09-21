#pragma once

namespace Blainn {
	// ScriptingSubsystem
	class Bifrost{
	public:
        static Bifrost& GetInstance();

		static void Init();
		static void Destroy();
		// TODO:
    private:
        Bifrost() = delete; 
        Bifrost(const Bifrost&) = delete;
        Bifrost& operator=(const Bifrost&) = delete; 
		Bifrost(const Bifrost&&) = delete;
        Bifrost& operator=(const Bifrost&&) = delete; 

		static bool m_isInitialized;
	};
}