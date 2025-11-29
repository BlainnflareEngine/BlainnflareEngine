#include "pch.h"

#include "aliases.h"

#include "components/ScriptingComponent.h"
#include "scene/Entity.h"
#include "scripting/LuaScript.h"
#include "scripting/TypeRegistration.h"
#include "subsystems/ScriptingSubsystem.h"

using namespace Blainn;

#ifdef BLAINN_REGISTER_LUA_TYPES

void Blainn::RegisterScriptingTypes(sol::state &luaState)
{
    sol::table scriptTable = luaState.create_table();

    scriptTable.set_function("SetLuaScriptsFolder", [](const std::string &path)
                             { ScriptingSubsystem::SetLuaScriptsFolder(eastl::string(path.c_str())); });

    scriptTable.set_function("CreateAttachScriptingComponent",
                             [](Entity entity) { ScriptingSubsystem::CreateAttachScriptingComponent(entity); });
    scriptTable.set_function("DestroyScriptingComponent",
                             [](Entity entity) { ScriptingSubsystem::DestroyScriptingComponent(entity); });

    scriptTable.set_function("LoadScript",
                             [&luaState](Entity entity, const std::string &path, bool callOnStart)
                             {
                                 sol::state_view lua(luaState);
                                 auto result =
                                     ScriptingSubsystem::LoadScript(entity, eastl::string(path.c_str()), callOnStart);
                                 if (!result.has_value()) return sol::object(sol::nil);
                                 return sol::make_object(lua, result.value().str());
                             });

    scriptTable.set_function("UnloadScript", [](const std::string &idStr)
                             { ScriptingSubsystem::UnloadScript(uuid::fromStrFactory(idStr)); });

    // Helper to list scripts on an entity
    scriptTable.set_function("ListScripts",
                             [&luaState](Entity entity)
                             {
                                 sol::state_view lua(luaState);
                                 sol::table tbl = lua.create_table();
                                 auto comp = entity.TryGetComponent<ScriptingComponent>();
                                 if (!comp) return tbl;
                                 int idx = 1;
                                 for (auto &kv : comp->scripts)
                                     tbl[idx++] = kv.first.str();
                                 return tbl;
                             });

    luaState["Scripting"] = scriptTable;

    // Register LuaScript usertype
    sol::usertype<LuaScript> LuaScriptType = luaState.new_usertype<LuaScript>("LuaScript", sol::default_constructor);
    LuaScriptType.set_function("Load", [](LuaScript &s, const std::string &path)
                               { return s.Load(eastl::string_view(path.c_str())); });
    LuaScriptType.set_function("IsLoaded", &LuaScript::IsLoaded);
    LuaScriptType.set_function("GetScriptPath", [](LuaScript &s) { return std::string(s.GetScriptPath().c_str()); });
    LuaScriptType.set_function("GetId", [](LuaScript &s) { return s.GetId().str(); });
    LuaScriptType.set_function("HasFunction", [](LuaScript &s, const std::string &funcName)
                               { return s.HasFunction(eastl::string(funcName.c_str())); });
    LuaScriptType.set_function("OnStartCall", &LuaScript::OnStartCall);
    LuaScriptType.set_function("OnUpdateCall", &LuaScript::OnUpdateCall);
    LuaScriptType.set_function("OnDestroyCall", &LuaScript::OnDestroyCall);
}

#endif