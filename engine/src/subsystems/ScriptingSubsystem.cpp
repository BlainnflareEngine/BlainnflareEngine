#include "pch.h"

#include "subsystems/ScriptingSubsystem.h"

#include <cassert>

#include "ScriptingSubsystem.h"
#include "tools/random.h"

using namespace Blainn;

void ScriptingSubsystem::Init()
{
    m_lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::string, sol::lib::table,
                         sol::lib::os, sol::lib::io);

    RegisterBlainnTypes();
    m_isInitialized = true;
}

void ScriptingSubsystem::Destroy()
{
    for (const auto &[scriptUuid, script] : m_scriptEntityConnections)
    {
        ScriptingComponent *component = m_scriptEntityConnections.at(scriptUuid).TryGetComponent<ScriptingComponent>();
        if (!component) continue;
        for (auto &script : component->scripts)
        {
            script.second.OnDestroyCall();
        }
    }
    m_scriptEntityConnections = {};
}

void ScriptingSubsystem::Update(Scene &scene, float deltaTimeMs)
{
    auto view = scene.GetAllEntitiesWith<ScriptingComponent>();
    for (const auto &[entity, scriptingComponent] : view.each())
    {
        for (auto &script : scriptingComponent.scripts)
        {
            script.second.OnUpdateCall(deltaTimeMs);
        }
    }
}

sol::state &ScriptingSubsystem::GetLuaState()
{
    return m_lua;
}
void ScriptingSubsystem::SetLuaScriptsFolder(const eastl::string &path)
{
    m_luaScriptsFolder = path;
};

eastl::optional<uuid> ScriptingSubsystem::LoadScript(Entity entity, const eastl::string &path, bool callOnStart)
{
    ScriptingComponent *component = entity.TryGetComponent<ScriptingComponent>();
    if (!component)
    {
        BF_ERROR("Script load error: entity " + entity.GetUUID().str() + "does not have scripting component");
        return eastl::optional<uuid>();
    }

    eastl::string scriptLoadPath = m_luaScriptsFolder + path;
    if (!std::filesystem::exists(scriptLoadPath.c_str()))
    {
        BF_ERROR("Script load error: script" + scriptLoadPath + "does not exist");
        return eastl::optional<uuid>();
    }

    eastl::unordered_map<uuid, LuaScript> &scripts = component->scripts;
    LuaScript luaScript;
    if (!luaScript.Load(scriptLoadPath)) return eastl::optional<uuid>();
    if (callOnStart) luaScript.OnStartCall();

    uuid scriptUuid = luaScript.GetId();
    scripts[scriptUuid] = std::move(luaScript);
    m_scriptEntityConnections[scriptUuid] = entity;
    return eastl::optional(eastl::move(scriptUuid));
}

void ScriptingSubsystem::UnloadScript(const uuid &scriptUuid)
{
    if (!m_scriptEntityConnections.contains(scriptUuid)) return;

    ScriptingComponent *component = m_scriptEntityConnections.at(scriptUuid).TryGetComponent<ScriptingComponent>();
    if (!component)
    {
        BF_ERROR("Script" + scriptUuid.str() + " unload error - component not exist");
        return;
    }

    m_scriptEntityConnections.erase(scriptUuid);
    eastl::unordered_map<uuid, LuaScript> &scripts = component->scripts;
    if (scripts.contains(scriptUuid))
    {
        LuaScript &script = scripts.at(scriptUuid);
        script.OnDestroyCall();
        scripts.erase(scriptUuid);
    }
    else
    {
        BF_ERROR("Script" + scriptUuid.str() + " unload error - not found");
    }
}

void Blainn::ScriptingSubsystem::RegisterBlainnTypes()
{
    sol::usertype<Vec2> Vec2Type = m_lua.new_usertype<Vec2>(
        "Vec2", sol::constructors<Vec2(), Vec2(float, float)>(), sol::meta_function::subtraction,
        [](const Vec2 &a, const Vec2 &b) { return a - b; }, sol::meta_function::addition,
        [](const Vec2 &a, const Vec2 &b) { return a + b; }, sol::meta_function::unary_minus, [](const Vec2 &a)
        { return -a; }, sol::meta_function::multiplication, [](const Vec2 &a, float s) { return a * s; },
        sol::meta_function::division, [](const Vec2 &a, float s) { return a / s; }, sol::meta_function::equal_to,
        [](const Vec2 &a, const Vec2 &b) { return a == b; });

    Vec2Type["x"] = &Vec2::x;
    Vec2Type["y"] = &Vec2::y;

    Vec2Type["Length"] = &Vec2::Length;
    Vec2Type["LengthSquared"] = &Vec2::LengthSquared;
    Vec2Type["Normalize"] = static_cast<void (Vec2::*)()>(&Vec2::Normalize);
    Vec2Type["Dot"] = &Vec2::Dot;

    Vec2Type["Distance"] = &Vec2::Distance;
    Vec2Type["Clamp"] = static_cast<void (Vec2::*)(const Vec2 &, const Vec2 &)>(&Vec2::Clamp);
    Vec2Type["Lerp"] = static_cast<Vec2 (*)(const Vec2 &, const Vec2 &, float)>(&Vec2::Lerp);
}
