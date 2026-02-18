#include "pch.h"

#include "aliases.h"

#include "scripting/TypeRegistration.h"
#include "spdlog/spdlog.h"
#include "subsystems/Log.h"
#include "subsystems/ScriptingSubsystem.h"

using namespace Blainn;

#ifdef BLAINN_REGISTER_LUA_TYPES

void Blainn::RegisterCommonTypes(sol::state &luaState)
{
    RegisterVectorTypes(luaState);
    RegisterQuatType(luaState);
    RegisterMat4Type(luaState);

    // Register Log functions
    sol::table logTable = luaState.create_table();

    logTable.set_function("Trace",
        [](const std::string &msg)
        {
            auto logger = spdlog::get(BLAINN_DEFAULT_LOGGER_NAME);
            if (logger) logger->trace(msg);
        });

    logTable.set_function("Debug",
        [](const std::string &msg)
        {
            auto logger = spdlog::get(BLAINN_DEFAULT_LOGGER_NAME);
            if (logger) logger->debug(msg);
        });

    logTable.set_function("Info",
        [](const std::string &msg)
        {
            auto logger = spdlog::get(BLAINN_DEFAULT_LOGGER_NAME);
            if (logger) logger->info(msg);
        });

    logTable.set_function("Warn",
        [](const std::string &msg)
        {
            auto logger = spdlog::get(BLAINN_DEFAULT_LOGGER_NAME);
            if (logger) logger->warn(msg);
        });

    logTable.set_function("Error",
        [](const std::string &msg)
        {
            auto logger = spdlog::get(BLAINN_DEFAULT_LOGGER_NAME);
            if (logger) logger->error(msg);
        });

    logTable.set_function("Fatal",
        [](const std::string &msg)
        {
            auto logger = spdlog::get(BLAINN_DEFAULT_LOGGER_NAME);
            if (logger) logger->critical(msg);
        });

    luaState["Log"] = logTable;
}

#endif
