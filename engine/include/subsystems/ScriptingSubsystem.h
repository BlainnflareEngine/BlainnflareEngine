#pragma once

namespace Blainn {
	class ScriptingSubsystem{
	public:
        static ScriptingSubsystem& GetInstance();

		static void Init();
		static void Destroy();
		// TODO:
    private:
        ScriptingSubsystem() = delete; 
        ScriptingSubsystem(const ScriptingSubsystem&) = delete;
        ScriptingSubsystem& operator=(const ScriptingSubsystem&) = delete; 
		ScriptingSubsystem(const ScriptingSubsystem&&) = delete;
        ScriptingSubsystem& operator=(const ScriptingSubsystem&&) = delete; 

		static bool m_isInitialized;
	};
}