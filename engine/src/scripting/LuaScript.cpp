#include "pch.h"

#include "scripting/LuaScript.h"

#include "subsystems/ScriptingSubsystem.h"
#include "tools/random.h"

using namespace Blainn;

LuaScript::LuaScript()
{
    m_id = Rand::getRandomUUID();
}

bool LuaScript::Load(const Path &scriptPath)
{
    m_scriptPath = scriptPath;

    sol::state &lua = ScriptingSubsystem::GetLuaState();
    sol::load_result script = lua.load_file(scriptPath.string());
    if (!script.valid())
    {
        sol::error err = script;
        BF_ERROR("Failed to load Lua script: {}\nError: {}", m_scriptPath.string(), err.what());
        return false;
    }

    sol::protected_function scriptAsFunc = script.get<sol::protected_function>();
    m_environment = sol::environment(lua, sol::create, lua.globals());
    sol::set_environment(m_environment, scriptAsFunc);
    // load lua functions to environment
    sol::protected_function_result result = script();
    if (!result.valid())
    {
        sol::error err = result;
        BF_ERROR("Failed to execute Lua script: {}\nError: {}", m_scriptPath.string(), err.what());
        return false;
    }

    if (HasFunction(PredefinedFunctions::OnStart)) m_predefinedFunctions.insert(PredefinedFunctions::OnStart);
    if (HasFunction(PredefinedFunctions::OnUpdate)) m_predefinedFunctions.insert(PredefinedFunctions::OnUpdate);
    if (HasFunction(PredefinedFunctions::OnDestroy)) m_predefinedFunctions.insert(PredefinedFunctions::OnDestroy);

    m_isLoaded = true;
    return true;
}

bool LuaScript::IsLoaded() const
{
    return m_isLoaded;
}

const Path &LuaScript::GetScriptPath() const
{
    return m_scriptPath;
}

const uuid &Blainn::LuaScript::GetId() const
{
    return m_id;
}

bool Blainn::LuaScript::HasFunction(const eastl::string &functionName) const
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
    if (!m_predefinedFunctions.contains(PredefinedFunctions::OnStart)) return false;
    return CustomCall(PredefinedFunctions::OnStart);
}

bool LuaScript::OnUpdateCall(float deltaTimeMs)
{
    if (!m_predefinedFunctions.contains(PredefinedFunctions::OnUpdate)) return false;
    return CustomCall(PredefinedFunctions::OnUpdate, deltaTimeMs);
}

bool LuaScript::OnDestroyCall()
{
    if (!m_predefinedFunctions.contains(PredefinedFunctions::OnDestroy)) return false;
    return CustomCall(PredefinedFunctions::OnDestroy);
}
