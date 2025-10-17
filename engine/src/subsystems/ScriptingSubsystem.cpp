#include "subsystems/ScriptingSubsystem.h"

#include "ScriptingSubsystem.h"
#include <cassert>

using namespace Blainn;

void ScriptingSubsystem::Init()
{
    lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::string, sol::lib::table,
                       sol::lib::os, sol::lib::io);

    m_isInitialized = true;
}

void ScriptingSubsystem::Destroy()
{
}

void Blainn::ScriptingSubsystem::Update(float deltaTimeMs)
{
    // TODO: implement
}

sol::state &ScriptingSubsystem::GetLuaState()
{
    return lua;
};
