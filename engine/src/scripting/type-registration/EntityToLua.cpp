#include "pch.h"

#include "aliases.h"

#include "scripting/TypeRegistration.h"

#include "scene/BasicComponents.h"
#include "scene/Entity.h"
#include "scene/Scene.h"

using namespace Blainn;

void Blainn::RegisterEntityTypes(sol::state &luaState)
{
    sol::usertype<Entity> EntityType = luaState.new_usertype<Entity>("Entity", sol::no_constructor);

    EntityType.set_function("IsValid", &Entity::IsValid);
    EntityType.set_function("GetUUID", [](Entity &e) { return e.GetUUID().str(); });
    EntityType.set_function("GetName", [](Entity &e) { return std::string(e.Name().c_str()); });

    EntityType.set_function("GetParentUUID", [](Entity &e) { return e.GetParentUUID().str(); });
    EntityType.set_function("SetParent", [](Entity &e, Entity parent) { e.SetParent(parent); });

    EntityType.set_function("Children",
                            [](Entity &e)
                            {
                                sol::state_view lua(sol::main_state);
                                sol::table tbl = lua.create_table();
                                int idx = 1;
                                for (const auto &child : e.Children())
                                    tbl[idx++] = child.str();
                                return tbl;
                            });

    EntityType.set_function("IsAncestorOf", &Entity::IsAncestorOf);
    EntityType.set_function("IsDescendantOf", &Entity::IsDescendantOf);
}