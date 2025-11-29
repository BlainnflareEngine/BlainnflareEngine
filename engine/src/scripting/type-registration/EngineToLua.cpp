#include "pch.h"

#include "aliases.h"

#include "Engine.h"
#include "scene/Scene.h"
#include "scripting/TypeRegistration.h"
#include "subsystems/AssetManager.h"
#include "subsystems/ScriptingSubsystem.h"

using namespace Blainn;

#ifdef BLAINN_REGISTER_LUA_TYPES

void Blainn::RegisterEngineTypes(sol::state &luaState)
{
    sol::table engineTbl = luaState.create_table();

    engineTbl.set_function("GetContentDirectory", []() { return Engine::GetContentDirectory().string(); });
    engineTbl.set_function("SetContentDirectory", [](const std::string &p) { Engine::SetContentDirectory(Path(p)); });
    engineTbl.set_function("SetDefaultContentDirectory", []() { Engine::SetDefaultContentDirectory(); });

    engineTbl.set_function("GetActiveScene",
                           []() -> Scene *
                           {
                               auto s = Engine::GetActiveScene();
                               return s ? s.get() : nullptr;
                           });
    engineTbl.set_function("ClearActiveScene", []() { Engine::ClearActiveScene(); });

    luaState["Engine"] = engineTbl;
}

#endif
