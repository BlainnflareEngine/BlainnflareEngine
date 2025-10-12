#pragma once

namespace Blainn
{
class ScriptingSubsystem
{
public:
    static void Init();
    static void Destroy();

    static void Update(float deltaTimeMs);

private:
    ScriptingSubsystem() = delete;
    ScriptingSubsystem(const ScriptingSubsystem &) = delete;
    ScriptingSubsystem &operator=(const ScriptingSubsystem &) = delete;
    ScriptingSubsystem(const ScriptingSubsystem &&) = delete;
    ScriptingSubsystem &operator=(const ScriptingSubsystem &&) = delete;

    inline static bool m_isInitialized = false;
};
} // namespace Blainn