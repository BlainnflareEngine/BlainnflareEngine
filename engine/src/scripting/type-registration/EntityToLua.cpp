#include "pch.h"

#include "aliases.h"

#include "scripting/TypeRegistration.h"

#include "components/MeshComponent.h"
#include "components/PhysicsComponent.h"
#include "components/ScriptingComponent.h"
#include "components/CameraComponent.h"
#include "handles/Handle.h"
#include "scene/BasicComponents.h"
#include "scene/Entity.h"
#include "scene/Scene.h"
#include "subsystems/ScriptingSubsystem.h"

using namespace Blainn;

#ifdef BLAINN_REGISTER_LUA_TYPES

void Blainn::RegisterEntityTypes(sol::state &luaState)
{
    sol::usertype<Entity> EntityType = luaState.new_usertype<Entity>("Entity", sol::no_constructor);

    EntityType.set_function("IsValid", &Entity::IsValid);
    EntityType.set_function("GetUUID", [](Entity &e) { return e.GetUUID().str(); });
    EntityType.set_function("GetName", [](Entity &e) { return std::string(e.Name().c_str()); });

    EntityType.set_function("GetParentUUID", [](Entity &e) { return e.GetParentUUID().str(); });
    EntityType.set_function("SetParent", [](Entity &e, Entity parent) { e.SetParent(parent); });

    EntityType.set_function("Children",
                            [&luaState](Entity &e)
                            {
                                sol::table tbl = luaState.create_table();
                                int idx = 1;
                                for (const auto &child : e.Children())
                                    tbl[idx++] = child.str();
                                return tbl;
                            });

    EntityType.set_function("IsAncestorOf", &Entity::IsAncestorOf);
    EntityType.set_function("IsDescendantOf", &Entity::IsDescendantOf);

    // Component helpers (Add/Get/Has/Remove) for common components
    EntityType.set_function("AddTagComponent", [](Entity &e, const std::string &tag)
                            { return e.AddComponent<TagComponent>(eastl::string(tag.c_str())); });
    EntityType.set_function("GetTagComponent",
                            [](Entity &e) -> TagComponent & { return e.GetComponent<TagComponent>(); });
    EntityType.set_function("HasTagComponent", [](Entity &e) { return e.HasComponent<TagComponent>(); });
    EntityType.set_function("RemoveTagComponent", [](Entity &e) { return e.RemoveComponentIfExists<TagComponent>(); });

    EntityType.set_function("AddTransformComponent", [](Entity &e) { return e.AddComponent<TransformComponent>(); });
    EntityType.set_function("GetTransformComponent",
                            [](Entity &e) -> TransformComponent & { return e.GetComponent<TransformComponent>(); });
    EntityType.set_function("HasTransformComponent", [](Entity &e) { return e.HasComponent<TransformComponent>(); });
    EntityType.set_function("RemoveTransformComponent",
                            [](Entity &e) { return e.RemoveComponentIfExists<TransformComponent>(); });

    // EntityType.set_function("AddMeshComponent", [](Entity &e, unsigned int meshIndex)
    //                         { return e.AddComponent<MeshComponent>(eastl::make_shared<MeshHandle>(meshIndex)); });
    EntityType.set_function("GetMeshComponent",
                            [](Entity &e) -> MeshComponent * { return e.TryGetComponent<MeshComponent>(); });
    EntityType.set_function("HasMeshComponent", [](Entity &e) { return e.HasComponent<MeshComponent>(); });
    EntityType.set_function("RemoveMeshComponent",
                            [](Entity &e) { return e.RemoveComponentIfExists<MeshComponent>(); });

    EntityType.set_function("AddScriptingComponent", [](Entity &e) { return e.AddComponent<ScriptingComponent>(); });
    EntityType.set_function("GetScriptingComponent",
                            [](Entity &e) -> ScriptingComponent * { return e.TryGetComponent<ScriptingComponent>(); });
    EntityType.set_function("HasScriptingComponent", [](Entity &e) { return e.HasComponent<ScriptingComponent>(); });
    EntityType.set_function("RemoveScriptingComponent",
                            [](Entity &e) { return e.RemoveComponentIfExists<ScriptingComponent>(); });

    EntityType.set_function("AddCameraComponent", [](Entity &e) { return e.AddComponent<CameraComponent>(); });
    EntityType.set_function("GetCameraComponent",
                            [](Entity &e) -> CameraComponent * { return e.TryGetComponent<CameraComponent>(); });
    EntityType.set_function("HasCameraComponent", [](Entity &e) { return e.HasComponent<CameraComponent>(); });
    EntityType.set_function("RemoveCameraComponent",
                            [](Entity &e) { return e.RemoveComponentIfExists<CameraComponent>(); });

    EntityType.set_function("AddPhysicsComponent",
                            [](Entity &e, const std::string &parentId, const std::string &componentId)
                            {
                                // PhysicsComponent requires a BodyBuilder reference; we can only add if BodyBuilder not
                                // required. Skip actual add.
                                return nullptr;
                            });
    EntityType.set_function("GetPhysicsComponent",
                            [](Entity &e) -> PhysicsComponent * { return e.TryGetComponent<PhysicsComponent>(); });
}

#endif