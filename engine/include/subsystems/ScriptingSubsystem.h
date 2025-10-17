#pragma once

#include <sol/sol.hpp>

namespace Blainn
{
class ScriptingSubsystem
{
public:
    static void Init();
    static void Destroy();

    static void Update(float deltaTimeMs);

    static sol::state &GetLuaState();

private:
    ScriptingSubsystem() = delete;
    ScriptingSubsystem(const ScriptingSubsystem &) = delete;
    ScriptingSubsystem &operator=(const ScriptingSubsystem &) = delete;
    ScriptingSubsystem(const ScriptingSubsystem &&) = delete;
    ScriptingSubsystem &operator=(const ScriptingSubsystem &&) = delete;

    inline static bool m_isInitialized = false;

    inline static sol::state lua = sol::state();

    // TODO: change? or make configurable?
    const char *LUA_SCRIPTS_DIR = "content/scripts/";
};
} // namespace Blainn