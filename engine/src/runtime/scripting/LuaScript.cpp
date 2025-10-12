#include "runtime/scripting/LuaScript.h"
#include "pch.h"

#include "runtime/scripting/scriptingCommon.h"

using namespace Blainn;

void LuaScript::Load(eastl::string_view scriptPath)
{
    assert(!m_isLoaded && "Script already loaded");

    if (m_isLoaded)
    {
        BF_ERROR("LuaScript::Load: Script already loaded: " + m_scriptPath);
        return;
    }

    m_scriptPath = scriptPath;

    sol::state &lua = GetLuaState();
    m_script = lua.load_file(scriptPath.data());
    if (!script.valid())
    {
        sol::error err = script;
        BF_ERROR("Failed to load Lua script: " + m_scriptPath + "\nError: " + err.what());
        return;
    }
    m_isLoaded = true;
}

bool LuaScript::IsLoaded() const
{
    return m_isLoaded;
}

LuaScriptType LuaScript::GetScriptType() const
{
    return m_scriptType;
}

const eastl::string &LuaScript::GetScriptPath() const
{
    return m_scriptPath;
}

bool LuaScript::onStartCall()
{
    assert(!m_OnStartCalled && "OnStart already called");

    if (!m_isLoaded)
    {
        BF_ERROR("LuaScript::onStartCall: Script not loaded: " + m_scriptPath);
        return false;
    }

    if (m_OnStartCalled)
    {
        BF_ERROR("LuaScript::onStartCall: OnStart already called for script " + m_scriptPath);
        return false;
    }

    sol::protected_function onStartFunc = m_script.get<sol::protected_function>("OnStart");
    if (!onStartFunc.valid())
    {
        BF_DEBUG("LuaScript::onStartCall: OnStart function not found in script " + m_scriptPath);
        return false;
    }

    sol::protected_function_result result = onStartFunc();
    if (!result.valid())
    {
        sol::error err = result;
        BF_ERROR("LuaScript::onStartCall: Error calling OnStart in script " + m_scriptPath + "\nError: " + err.what());
        return false;
    }

    m_OnStartCalled = true;
    return true;
}

bool LuaScript::OnUpdateCall(float deltaTimeMs)
{
    assert(m_isLoaded && "Script not loaded");
    assert(m_OnStartCalled && "OnStart not called");

    if (!m_isLoaded)
    {
        BF_ERROR("LuaScript::OnUpdateCall: Script not loaded: " + m_scriptPath);
        return false;
    }

    sol::protected_function onStartFunc = m_script.get<sol::protected_function>("OnUpdate");
    if (!onStartFunc.valid())
    {
        BF_DEBUG("LuaScript::OnUpdateCall: OnUpdate function not found in script " + m_scriptPath);
        return false;
    }
    sol::protected_function_result result = onStartFunc(deltaTimeMs);
    if (!result.valid())
    {
        sol::error err = result;
        BF_ERROR("LuaScript::OnUpdateCall: Error calling OnUpdate in script " + m_scriptPath
                 + "\nError: " + err.what());
        return false;
    }

    return true;
}

bool LuaScript::OnDestroyCall()
{
    assert(m_isLoaded && "Script not loaded");
    assert(m_OnStartCalled && "OnStart not called");
    assert(!m_OnDestroyCalled && "OnDestroy already called");

    if (!m_isLoaded)
    {
        BF_ERROR("LuaScript::OnDestroyCall: Script not loaded: " + m_scriptPath);
        return false;
    }

    if (!m_OnStartCalled)
    {
        BF_ERROR("LuaScript::OnDestroyCall: OnStart not called for script " + m_scriptPath);
        return false;
    }

    if (m_OnDestroyCalled)
    {
        BF_ERROR("LuaScript::OnDestroyCall: OnDestroy already called for script " + m_scriptPath);
        return false;
    }
    sol::protected_function onDestroyFunc = m_script.get<sol::protected_function>("OnDestroy");
    if (!onDestroyFunc.valid())
    {
        BF_DEBUG("LuaScript::OnDestroyCall: OnDestroy function not found in script " + m_scriptPath);
        return false;
    }
    sol::protected_function_result result = onDestroyFunc();
    if (!result.valid())
    {
        sol::error err = result;
        BF_ERROR("LuaScript::OnDestroyCall: Error calling OnDestroy in script " + m_scriptPath
                 + "\nError: " + err.what());
        return false;
    }

    return true;
}

bool LuaScript::CustomCall(eastl::string_view functionName /*= ""*/)
{
    // TODO:
    return true;
}
