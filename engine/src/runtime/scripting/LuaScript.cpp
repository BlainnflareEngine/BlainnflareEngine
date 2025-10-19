#include "runtime/scripting/LuaScript.h"
#include "pch.h"

#include "subsystems/ScriptingSubsystem.h"
#include "tools/random.h"

using namespace Blainn;

LuaScript::LuaScript()
{
    m_id = Rand::getRandomUUID();
}

bool LuaScript::Load(eastl::string_view scriptPath)
{
    m_scriptPath = scriptPath;

    sol::state &lua = ScriptingSubsystem::GetLuaState();
    m_script = lua.load_file(scriptPath.data());
    if (!m_script.valid())
    {
        sol::error err = m_script;
        BF_ERROR("Failed to load Lua script: " + m_scriptPath + "\nError: " + err.what());
        return false;
    }

    m_environment = sol::environment(lua, sol::create, lua.globals());
    // load lua functions to environment
    sol::protected_function_result result = m_script();
    if (!result.valid())
    {
        sol::error err = result;
        BF_ERROR("Failed to execute Lua script: " + m_scriptPath + "\nError: " + err.what());
        return false;
    }

    m_isLoaded = true;
    return true;
}

bool LuaScript::IsLoaded() const
{
    return m_isLoaded;
}

const eastl::string &LuaScript::GetScriptPath() const
{
    return m_scriptPath;
}

const uuid &Blainn::LuaScript::GetId() const
{
    return m_id;
}

bool Blainn::LuaScript::HasFunction(const eastl::string &functionName)
{
    sol::protected_function customFunc = m_environment[functionName.data()];
    if (!customFunc.valid())
    {
        return false;
    }
    return true;
}

bool LuaScript::OnStartCall()
{
    return CustomCall("OnStart");
}

bool LuaScript::OnUpdateCall(float deltaTimeMs)
{
    return CustomCall("OnUpdate");
}

bool LuaScript::OnDestroyCall()
{
    return CustomCall("OnDestroy");
}
