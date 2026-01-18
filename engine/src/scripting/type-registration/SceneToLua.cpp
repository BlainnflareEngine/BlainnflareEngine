#include "pch.h"

#include "aliases.h"

#include "scripting/TypeRegistration.h"

#include "ImportAssetData.h"
#include "scene/Entity.h"
#include "scene/Scene.h"
#include "spdlog/spdlog.h"
#include "subsystems/AssetManager.h"
#include "subsystems/ScriptingSubsystem.h"
#include <atomic>
#include <functional>
#include <unordered_map>

using namespace Blainn;

#ifdef BLAINN_REGISTER_LUA_TYPES

// Listener handle storage for Lua -> Scene event listeners
inline static std::unordered_map<uint64_t, std::function<void()>> s_sceneListenerRemovers;
inline static std::atomic<uint64_t> s_sceneNextListenerId{1};

void Blainn::RegisterSceneTypes(sol::state &luaState)
{
    using namespace Blainn;

    // Register the SceneEvent enum
    luaState.new_enum<true>("SceneEventType", "EntityCreated", SceneEventType::EntityCreated, "EntityDestroyed",
                            SceneEventType::EntityDestroyed, "EntityChanged", SceneEventType::EntityChanged,
                            "SceneChanged", SceneEventType::SceneChanged);

    // Register SceneEvent class hierarchy so Lua can call methods on derived events via base
    sol::usertype<SceneEvent> LuaSceneEventType = luaState.new_usertype<SceneEvent>("SceneEvent", sol::no_constructor);
    LuaSceneEventType.set_function("GetEventType", &SceneEvent::GetEventType);

    // SceneChangedEvent
    sol::usertype<SceneChangedEvent> SceneChangedEventType = luaState.new_usertype<SceneChangedEvent>(
        "SceneChangedEvent", sol::no_constructor, sol::base_classes, sol::bases<SceneEvent>());
    SceneChangedEventType.set_function("GetName",
                                       [](SceneChangedEvent &e) { return std::string(e.GetName().c_str()); });

    // EntityEvent and derived types
    sol::usertype<EntityEvent> EntityEventType = luaState.new_usertype<EntityEvent>(
        "EntityEvent", sol::no_constructor, sol::base_classes, sol::bases<SceneEvent>());
    EntityEventType.set_function("GetEntity", &EntityEvent::GetEntity);
    EntityEventType.set_function("GetUUID", [](EntityEvent &e) { return e.GetUUID().str(); });
    EntityEventType.set_function("IsSceneChanged", &EntityEvent::IsSceneChanged);

    sol::usertype<EntityCreatedEvent> EntityCreatedEventType = luaState.new_usertype<EntityCreatedEvent>(
        "EntityCreatedEvent", sol::no_constructor, sol::base_classes, sol::bases<EntityEvent>());
    EntityCreatedEventType.set_function("GetParent", &EntityCreatedEvent::GetParent);
    EntityCreatedEventType.set_function("CreatedByEditor", &EntityCreatedEvent::CreatedByEditor);

    sol::usertype<EntityDestroyedEvent> EntityDestroyedEventType = luaState.new_usertype<EntityDestroyedEvent>(
        "EntityDestroyedEvent", sol::no_constructor, sol::base_classes, sol::bases<EntityEvent>());

    sol::usertype<EntityChangedEvent> EntityChangedEventType = luaState.new_usertype<EntityChangedEvent>(
        "EntityChangedEvent", sol::no_constructor, sol::base_classes, sol::bases<EntityEvent>());

    sol::usertype<EntityReparentedEvent> EntityReparentedEventType = luaState.new_usertype<EntityReparentedEvent>(
        "EntityReparentedEvent", sol::no_constructor, sol::base_classes, sol::bases<EntityChangedEvent>());

    // Register a Scene usertype (no constructor exposed to Lua)
    sol::usertype<Scene> SceneType = luaState.new_usertype<Scene>("Scene", sol::no_constructor);

    // Simple wrappers to handle conversions between std::string and eastl::string / uuid
    SceneType.set_function("CreateEntity", [](Scene &scene, const std::string &name, bool onSceneChanged)
                           { return scene.CreateEntity(eastl::string(name.c_str()), onSceneChanged); });

    SceneType.set_function("CreateChildEntity",
                           [](Scene &scene, Entity parent, const std::string &name, bool onSceneChanged)
                           { return scene.CreateChildEntity(parent, eastl::string(name.c_str()), onSceneChanged); });

    SceneType.set_function(
        "CreateEntityWithID",
        [](Scene &scene, const std::string &idStr, const std::string &name, bool shouldSort, bool onSceneChanged)
        {
            uuid id = uuid::fromStrFactory(idStr);
            return scene.CreateEntityWithID(id, eastl::string(name.c_str()), shouldSort, onSceneChanged);
        });

    // CreateEntities - convenience wrapper accepting a path to a YAML file
    SceneType.set_function("CreateEntities",
                           [](Scene &scene, const std::string &yamlFilePath, bool onSceneChanged)
                           {
                               try
                               {
                                   YAML::Node root = YAML::LoadFile(yamlFilePath);
                                   if (root && root.IsSequence())
                                   {
                                       scene.CreateEntities(root, onSceneChanged);
                                   }
                               }
                               catch (const YAML::Exception &e)
                               {
                                   BF_ERROR("CreateEntities YAML error: {}", e.what());
                               }
                           });

    SceneType.set_function("DestroyEntity", [](Scene &scene, Entity entity) { scene.SubmitToDestroyEntity(entity); });

    SceneType.set_function("TryGetEntityWithUUID", [](Scene &scene, const std::string &idStr)
                           { return scene.TryGetEntityWithUUID(uuid::fromStrFactory(idStr)); });

    SceneType.set_function("TryGetEntityWithTag", [](Scene &scene, const std::string &tag)
                           { return scene.TryGetEntityWithTag(eastl::string(tag.c_str())); });
    SceneType.set_function("TryGetDescendantEntityWithTag", [](Scene &scene, Entity entity, const std::string &tag)
                           { return scene.TryGetDescendantEntityWithTag(entity, eastl::string(tag.c_str())); });

    // TODO: register ImportMeshData in asset manager
    // Simple wrapper - accept path string; create simple ImportMeshData with default values
    SceneType.set_function("CreateAttachMeshComponent",
                           [](Scene &scene, Entity entity, const std::string &pathStr)
                           {
                               Path p(pathStr);
                               ImportMeshData data;
                               data.path = p; // Path is std::filesystem::path
                               data.id = uuid{};
                               data.convertToLH = false;
                               data.createMaterials = false;
                               scene.CreateAttachMeshComponent(entity, p, data);
                           });

    // GetAllEntitiesWith - expose a TagComponent-only helper as a Lua-friendly utility
    SceneType.set_function("GetAllEntitiesWithTagComponent",
                           [&luaState](Scene &scene)
                           {
                               sol::state_view lua(luaState);
                               sol::table result = lua.create_table();
                               int idx = 1;
                               auto view = scene.GetAllEntitiesWith<TagComponent>();
                               for (auto ent : view)
                               {
                                   result[idx++] = Entity(ent, const_cast<Scene *>(&scene));
                               }
                               return result;
                           });

    SceneType.set_function("ParentEntity",
                           [](Scene &scene, Entity entity, Entity parent) { scene.ParentEntity(entity, parent); });
    SceneType.set_function("UnparentEntity", [](Scene &scene, Entity entity, bool convertToWorldSpace)
                           { scene.UnparentEntity(entity, convertToWorldSpace); });

    SceneType.set_function("DuplicateEntity",
                           [](Scene &scene, Entity entity) { return scene.DuplicateEntity(entity); });

    SceneType.set_function("ConvertToLocalSpace",
                           [](Scene &scene, Entity entity) { scene.ConvertToLocalSpace(entity); });
    SceneType.set_function("ConvertToWorldSpace",
                           [](Scene &scene, Entity entity) { scene.ConvertToWorldSpace(entity); });
    SceneType.set_function("GetWorldSpaceTransformMatrix",
                           [](Scene &scene, Entity entity) { return scene.GetWorldSpaceTransformMatrix(entity); });
    SceneType.set_function("GetWorldSpaceTransform",
                           [](Scene &scene, Entity entity) { return scene.GetWorldSpaceTransform(entity); });

    // Scene event listener helpers: AddEventListener returns an id, RemoveEventListener removes by id
    SceneType.set_function("AddEventListener",
                           [&luaState](Scene &scene, int eventTypeInt, sol::function listener) -> uint64_t
                           {
                               SceneEventType eventType = static_cast<SceneEventType>(eventTypeInt);
                               sol::function luaListener = listener;
                               uint64_t id = s_sceneNextListenerId.fetch_add(1);

                               auto handle = scene.AddEventListener(
                                   eventType,
                                   [&luaState, luaListener](const eastl::shared_ptr<SceneEvent> &ev)
                                   {
                                       // build a small table for the event
                                       sol::state_view lua(luaState);
                                       sol::table tbl = lua.create_table();
                                       tbl["type"] = static_cast<int>(ev->GetEventType());

                                       sol::protected_function pfunc = luaListener;
                                       sol::protected_function_result result = pfunc(tbl);
                                       if (!result.valid())
                                       {
                                           sol::error err = result;
                                           BF_ERROR("Lua scene listener error: " + eastl::string(err.what()));
                                       }
                                   });

                               s_sceneListenerRemovers[id] = [&scene, eventType, handle]()
                               { scene.RemoveEventListener(eventType, handle); };
                               return id;
                           });

    SceneType.set_function("RemoveEventListener",
                           [](Scene &scene, int eventTypeInt, uint64_t id)
                           {
                               auto it = s_sceneListenerRemovers.find(id);
                               if (it == s_sceneListenerRemovers.end()) return;
                               it->second();
                               s_sceneListenerRemovers.erase(it);
                           });
}

#endif