#pragma once

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
void RegisterNavigationTypes(sol::state &luaState);

void RegisterVectorTypes(sol::state &luaState);
void RegisterVector2Type(sol::state &luaState);
void RegisterVector3Type(sol::state &luaState);
void RegisterVector4Type(sol::state &luaState);

void RegisterQuatType(sol::state &luaState);
void RegisterMat4Type(sol::state &luaState);

void RegisterUUIDType(sol::state &luaState);

// void RegisterUselessSimpleMathTypes(sol::state &luaState);
void RegisterPhysicsBodyGetter(sol::state &luaState, sol::table &physicsTable);
void RegisterPhysicsBodyUpdater(sol::state &luaState, sol::table &physicsTable);
#endif
} // namespace Blainn