#include "pch.h"

#include "aliases.h"

#include "scripting/TypeRegistration.h"

#include "ImportAssetData.h"
#include "file-system/Material.h"
#include "file-system/Model.h"
#include "file-system/Texture.h"
#include "file-system/TextureType.h"
#include "handles/Handle.h"
#include "subsystems/AssetManager.h"
#include "subsystems/ScriptingSubsystem.h"

using namespace Blainn;

#ifdef BLAINN_REGISTER_LUA_TYPES

void Blainn::RegisterAssetManagerTypes(sol::state &luaState)
{
    // Handle types
    sol::usertype<Handle> HandleType = luaState.new_usertype<Handle>("Handle", sol::no_constructor);
    HandleType.set_function("GetIndex", &Handle::GetIndex);

    sol::usertype<TextureHandle> TextureHandleType = luaState.new_usertype<TextureHandle>(
        "TextureHandle", sol::no_constructor, sol::base_classes, sol::bases<Handle>());
    TextureHandleType.set_function("GetTexture", &TextureHandle::GetTexture);

    sol::usertype<MaterialHandle> MaterialHandleType = luaState.new_usertype<MaterialHandle>(
        "MaterialHandle", sol::no_constructor, sol::base_classes, sol::bases<Handle>());
    MaterialHandleType.set_function("GetMaterial", &MaterialHandle::GetMaterial);

    sol::usertype<MeshHandle> MeshHandleType =
        luaState.new_usertype<MeshHandle>("MeshHandle", sol::no_constructor, sol::base_classes, sol::bases<Handle>());
    MeshHandleType.set_function("GetMesh", &MeshHandle::GetMesh);

    // File system types
    sol::usertype<Model> ModelType = luaState.new_usertype<Model>("Model", sol::no_constructor);
    ModelType.set_function("GetMeshesCount", [](Model &m) { return (int)m.GetMeshes().size(); });

    sol::usertype<Texture> TextureTypeLua = luaState.new_usertype<Texture>("Texture", sol::no_constructor);
    TextureTypeLua.set_function("GetPath", [](Texture &t) { return t.GetPath().string(); });

    sol::usertype<Material> MaterialType = luaState.new_usertype<Material>("Material", sol::no_constructor);
    MaterialType.set_function("GetPath", [](Material &m) { return m.GetPath().string(); });
    MaterialType.set_function("GetShader", [](Material &m) { return std::string(m.GetShader().c_str()); });
    MaterialType.set_function("HasTexture", &Material::HasTexture);

    // Register texture type enum for convenience
    luaState.new_enum<true>("TextureType", "NONE", TextureType::NONE, "ALBEDO", TextureType::ALBEDO, "NORMAL",
                            TextureType::NORMAL, "METALLIC", TextureType::METALLIC, "ROUGHNESS", TextureType::ROUGHNESS,
                            "AO", TextureType::AO, "OTHER", TextureType::OTHER);

    sol::table manager = luaState.create_table();

    manager.set_function("LoadMesh",
                         [](const std::string &relativePath, const ImportMeshData &data) -> unsigned int
                         {
                             Path p(relativePath);
                             auto handle = AssetManager::GetInstance().LoadMesh(p, data);
                             if (!handle) return 0u;
                             return handle->GetIndex();
                         });

    manager.set_function("HasMesh",
                         [](const std::string &path) { return AssetManager::GetInstance().HasMesh(Path(path)); });
    manager.set_function("HasTexture",
                         [](const std::string &path) { return AssetManager::GetInstance().HasTexture(Path(path)); });
    // manager.set_function("HasMaterial",
    //                      [](const std::string &path) { return AssetManager::GetInstance().HasMaterial(Path(path));
    //                      });

    manager.set_function("LoadTexture",
                         [](const std::string &path, TextureType type) -> unsigned int
                         {
                             Path p(path);
                             auto t = AssetManager::GetInstance().LoadTexture(p, type);
                             if (!t) return 0u;
                             return t->GetIndex();
                         });

    manager.set_function("LoadMaterial",
                         [](const std::string &path) -> unsigned int
                         {
                             Path p(path);
                             auto m = AssetManager::GetInstance().LoadMaterial(p);
                             if (!m) return 0u;
                             return m->GetIndex();
                         });

    manager.set_function("GetMeshPathByIndex",
                         [](unsigned int index) -> std::string
                         {
                             Model &m = AssetManager::GetInstance().GetMeshByIndex(index);
                             return m.GetPath().string();
                         });

    manager.set_function("GetTexturePathByIndex",
                         [](unsigned int index) -> std::string
                         {
                             Texture &t = AssetManager::GetInstance().GetTextureByIndex(index);
                             return t.GetPath().string();
                         });

    manager.set_function("GetMaterialPathByIndex",
                         [](unsigned int index) -> std::string
                         {
                             Material &m = AssetManager::GetInstance().GetMaterialByIndex(index);
                             return m.GetPath().string();
                         });

    manager.set_function("OpenScene", [](const std::string &path) { AssetManager::OpenScene(Path(path)); });
    manager.set_function("CreateScene", [](const std::string &path) { AssetManager::CreateScene(Path(path)); });
    manager.set_function("SceneExists",
                         [](const std::string &path) -> bool { return AssetManager::SceneExists(Path(path)); });

    luaState["AssetManager"] = manager;
}

#endif