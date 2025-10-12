#pragma once

#include <sol/sol.hpp>

namespace Blainn
{
// TODO: change? or make configurable?
constexpr const char *LUA_SCRIPTS_DIR = "assets/scripts/";

inline static sol::state &GetLuaState()
{
    static sol::state lua;
    static bool isInitialized(false);
    if (!isInitialized)
    {
        lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::string, sol::lib::table,
                           sol::lib::os, sol::lib::io);
        isInitialized = true;
    }
    return lua;
};
} // namespace Blainn