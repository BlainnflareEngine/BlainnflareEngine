#include "pch.h"

#include "aliases.h"

#include "scripting/TypeRegistration.h"

#include "ai/BTBuilder.h"

using namespace Blainn;

#ifdef BLAINN_REGISTER_LUA_TYPES
void Blainn::RegisterAITypes(sol::state &luaState)
{
    // Expose BTType to Lua
    luaState.new_enum<true>("BTType", "Sequence", BTType::Sequence, "Selector", BTType::Selector, "Action",
                            BTType::Action, "Negate", BTType::Negate, "Condition", BTType::Condition, "Error",
                            BTType::Error);
}
#endif