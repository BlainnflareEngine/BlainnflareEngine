#include "pch.h"

#include "aliases.h"

#include "common/ImportAssetData.h"
#include "file-system/Material.h"
#include "file-system/Model.h"
#include "file-system/Texture.h"
#include "file-system/TextureType.h"
#include "handles/Handle.h"
#include "scripting/TypeRegistration.h"
#include "subsystems/AssetLoader.h"
#include "subsystems/AssetManager.h"

using namespace Blainn;

void Blainn::RegisterAssetLoaderTypes(sol::state &luaState)
{
    // Expose TextureType to Lua
    luaState.new_enum<true>("TextureType", "NONE", TextureType::NONE, "ALBEDO", TextureType::ALBEDO, "NORMAL",
                            TextureType::NORMAL, "METALLIC", TextureType::METALLIC, "ROUGHNESS", TextureType::ROUGHNESS,
                            "AO", TextureType::AO, "OTHER", TextureType::OTHER);

    // ImportMeshData binding
    sol::usertype<ImportMeshData> ImportMeshDataType =
        luaState.new_usertype<ImportMeshData>("ImportMeshData", sol::constructors<ImportMeshData()>());
    ImportMeshDataType["path"] = &ImportMeshData::path;
    ImportMeshDataType["id"] =
        sol::property([](const ImportMeshData &d) { return d.id.str(); },
                      [](ImportMeshData &d, const std::string &idStr) { d.id = uuid::fromStrFactory(idStr); });
    ImportMeshDataType["convertToLH"] = &ImportMeshData::convertToLH;
    ImportMeshDataType["createMaterials"] = &ImportMeshData::createMaterials;

    // Register handle usertypes and file-system types for Lua access
    sol::usertype<Handle> HandleType = luaState.new_usertype<Handle>("Handle", sol::no_constructor);
    HandleType["GetIndex"] = &Handle::GetIndex;

    sol::usertype<TextureHandle> TextureHandleType =
        luaState.new_usertype<TextureHandle>("TextureHandle", sol::no_constructor, sol::bases<Handle>());
    TextureHandleType["GetTexture"] = &TextureHandle::GetTexture;

    sol::usertype<MaterialHandle> MaterialHandleType =
        luaState.new_usertype<MaterialHandle>("MaterialHandle", sol::no_constructor, sol::bases<Handle>());
    MaterialHandleType["GetMaterial"] = &MaterialHandle::GetMaterial;

    sol::usertype<MeshHandle> MeshHandleType =
        luaState.new_usertype<MeshHandle>("MeshHandle", sol::no_constructor, sol::bases<Handle>());
    MeshHandleType["GetMesh"] = &MeshHandle::GetMesh;

    // File-system types
    sol::usertype<Model> ModelType = luaState.new_usertype<Model>("Model", sol::no_constructor);
    ModelType["GetMeshCount"] = [](Model &m) { return (int)m.GetMeshes().size(); };

    sol::usertype<Material> MaterialType = luaState.new_usertype<Material>("Material", sol::no_constructor);
    MaterialType["GetShader"] = [](Material &m) { return std::string(m.GetShader().c_str()); };
    MaterialType["HasTexture"] = &Material::HasTexture;

    sol::usertype<Texture> TextureUser = luaState.new_usertype<Texture>("Texture", sol::no_constructor);
    // We don't expose GPU resource to Lua; provide basic info if necessary in future

    // Create table for AssetLoader/AssetManager helpers
    sol::table assetTable = luaState.create_table();

    assetTable.set_function("LoadMesh",
                            [](const std::string &relativePath, const ImportMeshData &data)
                            {
                                Path p(relativePath);
                                auto handle = AssetManager::GetInstance().LoadMesh(p, data);
                                if (!handle) return 0u;
                                return handle->GetIndex();
                            });

    assetTable.set_function("LoadTexture",
                            [](const std::string &pathStr, const TextureType type)
                            {
                                Path p(pathStr);
                                auto handle = AssetManager::GetInstance().LoadTexture(p, type);
                                if (!handle) return 0u;
                                return handle->GetIndex();
                            });

    assetTable.set_function("LoadMaterial",
                            [](const std::string &pathStr)
                            {
                                Path p(pathStr);
                                auto handle = AssetManager::GetInstance().LoadMaterial(p);
                                if (!handle) return 0u;
                                return handle->GetIndex();
                            });

    assetTable.set_function("GetMeshPath",
                            [](const unsigned int index) -> std::string
                            {
                                Model &m = AssetManager::GetInstance().GetMeshByIndex(index);
                                return m.GetPath().string();
                            });

    assetTable.set_function("GetModelByIndex", [](const unsigned int index) -> Model &
                            { return AssetManager::GetInstance().GetMeshByIndex(index); });

    assetTable.set_function("GetTextureByIndex", [](const unsigned int index) -> Texture &
                            { return AssetManager::GetInstance().GetTextureByIndex(index); });

    assetTable.set_function("GetMaterialByIndex", [](const unsigned int index) -> Material &
                            { return AssetManager::GetInstance().GetMaterialByIndex(index); });

    luaState["AssetManager"] = assetTable;
}