#pragma once

#include <EASTL/string.h>

#include <lua.hpp>
#include <sol/sol.hpp>

#include "aliases.h"

namespace Blainn
{
class LuaScript
{
public:
    LuaScript();

    bool Load(eastl::string_view scriptPath);
    bool IsLoaded() const;

    const eastl::string &GetScriptPath() const;
    const uuid &GetId() const;

    bool HasFunction(const eastl::string &functionName);
    bool OnStartCall();
    bool OnUpdateCall(float deltaTimeMs);
    bool OnDestroyCall();

    template <typename... Args> bool CustomCall(eastl::string_view functionName = "", Args &&...args)
    {
        if (!m_isLoaded)
        {
            BF_ERROR("LuaScript::CustomCall: Script not loaded: " + m_scriptPath);
            return false;
        }

        sol::protected_function customFunc = m_environment[functionName.data()];
        if (!customFunc.valid())
        {
            return false;
        }
        sol::set_environment(m_environment, customFunc);
        sol::protected_function_result result = customFunc(std::forward<Args>(args)...);
        if (!result.valid())
        {
            sol::error err = result;
            BF_ERROR("LuaScript::CustomCall: Error calling function " + eastl::string(functionName) + " in script "
                     + m_scriptPath + "\nError: " + err.what());
            return false;
        }

        return true;
    }

private:
    bool m_isLoaded = false;
    uuid m_id;
    eastl::string m_scriptPath;
    sol::environment m_environment;
};
} // namespace Blainn