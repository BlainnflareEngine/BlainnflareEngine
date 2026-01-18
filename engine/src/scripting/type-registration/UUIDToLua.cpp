//
// Created by gorev on 18.01.2026.
//
#ifdef BLAINN_REGISTER_LUA_TYPES

#include "scripting/TypeRegistration.h"
#include "aliases.h"

namespace Blainn
{
void RegisterUUIDType(sol::state &luaState)
{
    luaState.new_usertype<uuid>(
        "__UUID_internal", sol::meta_function::equal_to, [](const uuid &a, const uuid &b) { return a == b; },
        sol::meta_function::less_than, [](const uuid &a, const uuid &b) { return a < b; }, "String",
        [](const uuid &self) -> std::string { return self.str(); }, sol::meta_function::to_string,
        [](const uuid &self) -> std::string { return self.str(); });

    luaState["UUID"] = luaState.create_table_with(
        "New", [](const std::string &s) -> uuid { return uuid::fromStrFactory(s); }, "Nil", []() -> uuid
        { return uuid(); }, "FromString", [](const std::string &s) -> uuid { return uuid::fromStrFactory(s); });
}

} // namespace Blainn
#endif