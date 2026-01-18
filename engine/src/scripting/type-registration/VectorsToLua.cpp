#include "pch.h"

#include "aliases.h"

#include "scripting/TypeRegistration.h"

using namespace Blainn;

#ifdef BLAINN_REGISTER_LUA_TYPES

void Blainn::RegisterVectorTypes(sol::state &luaState)
{
    RegisterVector2Type(luaState);
    RegisterVector3Type(luaState);
    RegisterVector4Type(luaState);
}
#endif