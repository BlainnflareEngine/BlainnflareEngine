#pragma once

namespace sol
{
class state;
}

namespace Blainn
{
void RegisterCommonTypes(sol::state &luaState);
void RegisterInputTypes(sol::state &luaState);
void RegisterEntityTypes(sol::state &luaState);
void RegisterSceneTypes(sol::state &luaState);
void RegisterScriptingTypes(sol::state &luaState);
void RegisterAssetManagerTypes(sol::state &luaState);
void RegisterAssetLoaderTypes(sol::state &luaState);
void RegisterEngineTypes(sol::state &luaState);
} // namespace Blainn