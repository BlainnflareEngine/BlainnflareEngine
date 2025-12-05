#include "pch.h"

#include "subsystems/ScriptingSubsystem.h"

#include <cassert>
#include <sol/sol.hpp>


#include "Engine.h"
#include "ScriptingSubsystem.h"
#include "scene/Scene.h"
#include "scripting/TypeRegistration.h"

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
#ifdef BLAINN_TEST_LUA_SCRIPTS

    static bool create;
    if (!create)
    {
        m_scriptTestEntity = Engine::GetActiveScene()->CreateEntity("LuaScriptTestEntity");
        CreateAttachScriptingComponent(m_scriptTestEntity);
        //m_scriptTestUuid1 =
        //    ScriptingSubsystem::LoadScript(m_scriptTestEntity, "scripts/test1.lua", true).value_or(uuid());
        m_scriptTestUuid2 =
            ScriptingSubsystem::LoadScript(m_scriptTestEntity, "./scripts/test2.lua", true).value_or(uuid());

        create = true;
    }
#endif

    // TODO: can be replaced with profiler
    // const int num_tests = 10;
    // long long duration = 0;
    // for (int j = 0; j < num_tests; j++)
    // {
    //     std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    //     for (int i = 0; i < 1000; i++)
    //     {
    //         ScriptingSubsystem::Update(*s_activeScene, deltaTime);
    //     }
    //     std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    //     duration += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    // }
    // long long avg = duration / num_tests;
    // std::cout << avg << " avg microseconds" << std::endl;
    // exit(0);

    auto view = scene.GetAllEntitiesWith<ScriptingComponent>();
    for (const auto &[entity, scriptingComponent] : view.each())
    {
        for (auto &script : scriptingComponent.scripts)
        {
            script.second.OnUpdateCall(deltaTimeMs);
        }
    }
}

void Blainn::ScriptingSubsystem::CreateAttachScriptingComponent(Entity entity)
{
    ScriptingComponent *component = entity.TryGetComponent<ScriptingComponent>();
    if (component)
    {
        BF_ERROR("Entity " + entity.GetUUID().str() + " already has ScriptingComponent");
        return;
    }
    entity.AddComponent<ScriptingComponent>();
}

void Blainn::ScriptingSubsystem::DestroyScriptingComponent(Entity entity)
{
    ScriptingComponent *component = entity.TryGetComponent<ScriptingComponent>();
    if (!component)
    {
        return;
    }

    eastl::vector<uuid> scriptUuids;
    scriptUuids.reserve(component->scripts.size());
    eastl::transform(component->scripts.begin(), component->scripts.end(), eastl::back_inserter(scriptUuids),
                     [](const eastl::pair<uuid, LuaScript> &pair) { return pair.first; });

    for (const auto &scriptUuid : scriptUuids)
    {
        UnloadScript(scriptUuid);
    }

    entity.RemoveComponent<ScriptingComponent>();
}

sol::state &ScriptingSubsystem::GetLuaState()
{
    return m_lua;
}

eastl::optional<uuid> ScriptingSubsystem::LoadScript(Entity entity, const Path &path, bool callOnStart)
{
    ScriptingComponent *component = entity.TryGetComponent<ScriptingComponent>();
    if (!component)
    {
        BF_ERROR("Script load error: entity " + entity.GetUUID().str() + "does not have scripting component");
        return eastl::optional<uuid>();
    }

    Path scriptLoadPath = Engine::GetContentDirectory() / path;
    if (!std::filesystem::exists(scriptLoadPath.c_str()))
    {
        BF_ERROR("Script load error: script" + scriptLoadPath.string() + "does not exist");
        return eastl::optional<uuid>();
    }

    eastl::unordered_map<uuid, LuaScript> &scripts = component->scripts;
    LuaScript luaScript;
    if (!luaScript.Load(scriptLoadPath, entity)) return eastl::optional<uuid>();
    if (callOnStart) luaScript.OnStartCall();

    uuid scriptUuid = luaScript.GetId();
    scripts[scriptUuid] = eastl::move(luaScript);
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
#ifdef BLAINN_REGISTER_LUA_TYPES
    RegisterCommonTypes(m_lua);
    RegisterInputTypes(m_lua);
    RegisterAssetLoaderTypes(m_lua);
    RegisterComponentTypes(m_lua);
    RegisterEntityTypes(m_lua);
    RegisterSceneTypes(m_lua);
    RegisterAssetManagerTypes(m_lua);
    RegisterEngineTypes(m_lua);
    RegisterScriptingTypes(m_lua);
#endif
}
