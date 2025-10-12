#pragma once

#include "EASTL/string.h"

#include <lua.hpp>
#include <sol/sol.hpp>

namespace Blainn
{
enum class LuaScriptType
{
    FrameworkIntegration,
    Custom
};

class LuaScript
{
public:
    LuaScript() = default;
    void Load(eastl::string_view scriptPath);
    bool IsLoaded() const;

    LuaScriptType GetScriptType() const;
    const eastl::string &GetScriptPath() const;

    bool onStartCall();
    bool OnUpdateCall(float deltaTimeMs);
    bool OnDestroyCall();
    bool CustomCall(eastl::string_view functionName = "");

private:
    bool m_isLoaded = false;
    LuaScriptType m_scriptType = LuaScriptType::FrameworkIntegration;
    eastl::string m_scriptPath;
    sol::load_result m_script;
    sol::table m_env;

    // debug checks
    bool m_OnStartCalled = false;
    bool m_OnDestroyCalled = false;
};
} // namespace Blainn