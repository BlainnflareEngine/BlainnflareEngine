#pragma once

namespace sol
{
class state;
}

namespace Blainn
{
void RegisterCommonTypes(sol::state &luaState);
void RegisterInputTypes(sol::state &luaState);
} // namespace Blainn