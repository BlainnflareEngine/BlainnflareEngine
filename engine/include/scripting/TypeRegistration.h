#pragma once

#include "subsystems/ScriptingSubsystem.h"

namespace sol
{
class state;
}

namespace Blainn
{
#ifdef BLAINN_REGISTER_LUA_TYPES
void RegisterCommonTypes(sol::state &luaState);
void RegisterInputTypes(sol::state &luaState);
void RegisterEntityTypes(sol::state &luaState);
void RegisterSceneTypes(sol::state &luaState);
void RegisterComponentTypes(sol::state &luaState);
void RegisterScriptingTypes(sol::state &luaState);
void RegisterAssetManagerTypes(sol::state &luaState);
void RegisterAssetLoaderTypes(sol::state &luaState);
void RegisterEngineTypes(sol::state &luaState);
void RegisterPhysicsTypes(sol::state &luaState);
void RegisterAITypes(sol::state &luaState);
#endif
} // namespace Blainn