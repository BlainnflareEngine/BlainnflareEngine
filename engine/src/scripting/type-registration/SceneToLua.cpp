#include "pch.h"

#include "aliases.h"

#include "scripting/TypeRegistration.h"

#include "ImportAssetData.h"
#include "scene/Entity.h"
#include "scene/Scene.h"
#include "spdlog/spdlog.h"
#include "subsystems/AssetManager.h"
#include "subsystems/ScriptingSubsystem.h"

using namespace Blainn;

#ifdef BLAINN_REGISTER_LUA_TYPES

void Blainn::RegisterSceneTypes(sol::state &luaState)
{
    using namespace Blainn;

    // Register the SceneEvent enum
    luaState.new_enum<true>("SceneEventType", "EntityCreated", SceneEventType::EntityCreated, "EntityDestroyed",
                            SceneEventType::EntityDestroyed, "EntityChanged", SceneEventType::EntityChanged,
                            "SceneChanged", SceneEventType::SceneChanged);

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

    SceneType.set_function("SubmitToDestroyEntity",
                           [](Scene &scene, Entity entity) { scene.SubmitToDestroyEntity(entity); });

    // Multiple overloads for DestroyEntity
    SceneType.set_function("DestroyEntity", [](Scene &scene, Entity entity) { scene.SubmitToDestroyEntity(entity); });

    // UUID helpers - accept string UUIDs from Lua
    SceneType.set_function("GetEntityWithUUID", [](Scene &scene, const std::string &idStr)
                           { return scene.GetEntityWithUUID(uuid::fromStrFactory(idStr)); });
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
}

#endif