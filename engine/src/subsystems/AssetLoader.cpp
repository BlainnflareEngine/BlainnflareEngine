//
// Created by gorev on 30.09.2025.
//

#include <pch.h>

#include "AssetLoader.h"

#include "AssetManager.h"
#include "Engine.h"
#include "ImportAssetData.h"
#include "MeshData.h"
#include "file-system/Material.h"
#include "file-system/Model.h"
#include "file-system/Texture.h"

#include "Render/Device.h"
#include "Render/CommandQueue.h"

#include <DirectXTK12/Inc/DDSTextureLoader.h>
#include <DirectXTK12/Inc/ResourceUploadBatch.h>
#include <DirectXTK12/Inc/WICTextureLoader.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <filesystem>

namespace Blainn
{


void AssetLoader::Init()
{
    BF_INFO("AssetLoader Init");

    InitTextureOffsetsTable();
}


void AssetLoader::Destroy()
{
    BF_INFO("AssetLoader Destroy");
}


eastl::shared_ptr<Model> AssetLoader::ImportModel(const Path &relativePath, const ImportMeshData &data)
{
    assert(relativePath.is_relative());

    Path absolutePath = Engine::GetContentDirectory() / relativePath;
    if (absolutePath.empty())
    {
        BF_ERROR("AssetLoader ImportModel: path is empty");
    }

    Model model = Model(relativePath);
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(absolutePath.string(),
                                             aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_CalcTangentSpace
                                                 | aiProcess_FixInfacingNormals | aiProcess_ConvertToLeftHanded);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        BF_ERROR("AssetLoader ImportModel: error loading model " + std::string(importer.GetErrorString()));
        return eastl::make_shared<Model>(AssetManager::GetDefaultMesh()->GetMesh());
    }

    ProcessNode(relativePath, *scene->mRootNode, *scene, Mat4::Identity, model);

    // to merge together all meshes of the model
    model.CreateBufferResources();
    CreateModelGPUResources(model);

    return eastl::make_shared<Model>(model);
}

void AssetLoader::CreateModelGPUResources(Model &model)
{
    // Gpu stuff
    auto cmdQueue = Device::GetInstance().GetCommandQueue();
    auto cmdList = cmdQueue->GetDefaultCommandList();
    auto cmdAlloc = cmdQueue->GetDefaultCommandAllocator();
    cmdAlloc->Reset();
    cmdList->Reset(cmdAlloc.Get(), nullptr);

    model.CreateGPUBuffers(cmdList.Get());

    ThrowIfFailed(cmdList->Close());
    ID3D12CommandList *const ppCommandLists[] = {cmdList.Get()};
    cmdQueue->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    cmdQueue->Flush();

    model.m_bisLoaded = true;
}

void AssetLoader::ProcessNode(const Path &path, const aiNode &node, const aiScene &scene, const Mat4 &parentMatrix,
                              Model &model)
{
    Mat4 nodeTransform = Mat4(&node.mTransformation.a1).Transpose() * parentMatrix;

    for (size_t i = 0; i < node.mNumMeshes; ++i)
    {
        unsigned int meshIndex = node.mMeshes[i];
        auto mesh = scene.mMeshes[meshIndex];
        model.GetMeshes().emplace_back(ProcessMesh(path, *mesh, scene, node, nodeTransform, model));
    }

    for (size_t i = 0; i < node.mNumChildren; ++i)
    {
        ProcessNode(path, *node.mChildren[i], scene, nodeTransform, model);
    }
}


MeshData<> AssetLoader::ProcessMesh(const Path &path, const aiMesh &mesh, const aiScene &scene, const aiNode &node,
                                    const Mat4 &parentMatrix, Model &model)
{
    MeshData<> result_mesh = MeshData<>();
    result_mesh.parentMatrix = parentMatrix;

    for (size_t i = 0; i < mesh.mNumVertices; ++i)
    {
        BlainnVertex vertex;

        vertex.position = GetPosition(mesh, i);

        if (mesh.HasNormals()) vertex.normal = GetNormal(mesh, i);
        else vertex.normal = Vec3::Zero;

        if (mesh.HasTangentsAndBitangents())
        {
            vertex.tangent = GetTangent(mesh, i);
            vertex.bitangent = GetBitangent(mesh, i);
        }
        else
        {
            vertex.tangent = Vec3::Zero;
            vertex.bitangent = Vec3::Zero;
        }

        if (mesh.HasTextureCoords(i)) vertex.texCoord = GetTextCoords(mesh, i);

        result_mesh.vertices.emplace_back(vertex);
    }

    for (unsigned int i = 0; i < mesh.mNumFaces; ++i)
    {
        const aiFace face = mesh.mFaces[i];

        for (int j = 0; j < face.mNumIndices; ++j)
        {
            result_mesh.indices.push_back(face.mIndices[j]);
        }
    }


    // TODO: get material
    return result_mesh;
}


Vec3 AssetLoader::GetPosition(const aiMesh &mesh, const unsigned int meshIndex)
{
    return Vec3(mesh.mVertices[meshIndex].x, mesh.mVertices[meshIndex].y, mesh.mVertices[meshIndex].z);
}


Vec3 AssetLoader::GetNormal(const aiMesh &mesh, const unsigned int meshIndex)
{
    return Vec3(mesh.mNormals[meshIndex].x, mesh.mNormals[meshIndex].y, mesh.mNormals[meshIndex].z);
}


Vec3 AssetLoader::GetTangent(const aiMesh &mesh, const unsigned int meshIndex)
{
    return Vec3(mesh.mTangents[meshIndex].x, mesh.mTangents[meshIndex].y, mesh.mTangents[meshIndex].z);
}


Vec3 AssetLoader::GetBitangent(const aiMesh &mesh, const unsigned int meshIndex)
{
    return Vec3(mesh.mBitangents[meshIndex].x, mesh.mBitangents[meshIndex].y, mesh.mBitangents[meshIndex].z);
}


Vec2 AssetLoader::GetTextCoords(const aiMesh &mesh, const unsigned int meshIndex)
{
    return Vec2(mesh.mTextureCoords[0][meshIndex].x, mesh.mTextureCoords[0][meshIndex].y);
}


eastl::shared_ptr<Texture> AssetLoader::LoadTexture(const Path &path, const TextureType type)
{
    assert(path.is_relative());
    Microsoft::WRL::ComPtr<ID3D12Resource> temp;
    CreateTextureGPUResources(path, temp, type);

    return eastl::make_shared<Texture>(path, temp, type);
}

void AssetLoader::CreateTextureGPUResources(const Path &path, Microsoft::WRL::ComPtr<ID3D12Resource> &resource, TextureType type)
{
    assert(path.is_relative());

    auto device = Device::GetInstance().GetDevice2().Get();
    auto commandQueue = Device::GetInstance().GetCommandQueue();

    ResourceUploadBatch upload(device);
    upload.Begin();

    if (path.extension() == L".dds")
    {
        ThrowIfFailed(CreateDDSTextureFromFile(device, upload, (Engine::GetContentDirectory()/ path).wstring().c_str(), resource.ReleaseAndGetAddressOf()));
    }
    else if (path.extension() == L".png" || path.extension() == L".jpg")
    {
        ThrowIfFailed(CreateWICTextureFromFile(device, upload, (Engine::GetContentDirectory()/ path).wstring().c_str(), resource.ReleaseAndGetAddressOf()));
    }

    // Create default upload heap manually
    {
        /* auto desc = CD3DX12_RESOURCE_DESC(
            D3D12_RESOURCE_DIMENSION_TEXTURE2D, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, c_texture_size,
            c_texture_size, 1, 1, DXGI_FORMAT_R8G8_SNORM, 1, 0, D3D12_TEXTURE_LAYOUT_UNKNOWN,
        D3D12_RESOURCE_FLAG_NONE);

        CD3DX12_HEAP_PROPERTIES defaultHeapProperties(D3D12_HEAP_TYPE_DEFAULT);

        ThrowIfFailed(device->CreateCommittedResource(&defaultHeapProperties, D3D12_HEAP_FLAG_NONE, &desc,
                                                        D3D12_RESOURCE_STATE_COPY_DEST, nullptr,
                                                        IID_PPV_ARGS(&m_resource)));

        D3D12_SUBRESOURCE_DATA initData = {mydata, c_texture_size * 2, 0};
        upload.Upload(tex.Get(), 0, &initData, 1);

        upload.Transition(tex.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);*/
    }

    CreateTextureDescriptor(resource.Get(), type);

    // Upload the resources to the GPU.
    auto finish = upload.End(commandQueue->GetCommandQueue().Get());

    // Wait for the upload thread to terminate
    finish.wait();
}

void Blainn::AssetLoader::CreateTextureDescriptor(ID3D12Resource* textureRes, TextureType type)
{
    auto& device = Device::GetInstance();

    UINT freeTextureOffsetOfType = m_texturesOffsetsTable[type];
    UINT texturePlacementOffset = /*m_texturesSrvHeapStartIndex*/ 7u + (static_cast<UINT>(type) - 1u) * MAX_TEXTURES + freeTextureOffsetOfType;
    //texture.SetTextureDescriptorOffset(texturePlacementOffset);

    auto srvCpuStart = device.GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
    auto cbvSrvUavDescriptorSize = device.GetDescriptorHandleIncrementSize();

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    ZeroMemory(&srvDesc, sizeof(srvDesc));

    CD3DX12_CPU_DESCRIPTOR_HANDLE localHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(srvCpuStart, texturePlacementOffset, cbvSrvUavDescriptorSize);

    auto texD3DResource = textureRes;
    srvDesc.Format = texD3DResource->GetDesc().Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Texture2D.MostDetailedMip = 0u;
    srvDesc.Texture2D.MipLevels = texD3DResource->GetDesc().MipLevels;
    srvDesc.Texture2D.PlaneSlice;
    srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
    device.CreateShaderResourceView(texD3DResource, &srvDesc, localHandle);

    m_texturesOffsetsTable.at(type)++;
}

void Blainn::AssetLoader::InitTextureOffsetsTable()
{
    for (auto &textureOffset : m_texturesOffsetsTable)
    {
        textureOffset.second = 0u;
    }
}

eastl::shared_ptr<Material> AssetLoader::LoadMaterial(const Path &relativePath)
{
    assert(relativePath.is_relative());

    auto absolutePath = Engine::GetContentDirectory() / relativePath;
    YAML::Node config = YAML::LoadFile(absolutePath.string());

    auto shaderPath = config["ShaderPath"].as<std::string>();
    auto albedo = config["AlbedoPath"].as<std::string>();
    auto normal = config["NormalPath"].as<std::string>();
    auto metallic = config["MetallicPath"].as<std::string>();
    auto roughness = config["RoughnessPath"].as<std::string>();
    auto ambient = config["AOPath"].as<std::string>();

    auto albedoColor = config["AlbedoColor"].as<std::string>();
    auto normalScale = config["NormalScale"].as<float>();
    auto metallicScale = config["MetallicScale"].as<float>();
    auto roughnessScale = config["RoughnessScale"].as<float>();

    auto material = eastl::make_shared<Material>(relativePath, ToEASTLString(shaderPath));
    auto &manager = AssetManager::GetInstance();

    if (!albedo.empty())
        if (manager.HasTexture(albedo)) material->SetTexture(manager.GetTexture(albedo), TextureType::ALBEDO);
        else material->SetTexture(manager.LoadTexture(albedo, TextureType::ALBEDO), TextureType::ALBEDO);

    if (!normal.empty())
        if (manager.HasTexture(normal)) material->SetTexture(manager.GetTexture(normal), TextureType::NORMAL);
        else material->SetTexture(manager.LoadTexture(normal, TextureType::NORMAL), TextureType::NORMAL);

    if (!metallic.empty())
        if (manager.HasTexture(metallic)) material->SetTexture(manager.GetTexture(metallic), TextureType::METALLIC);
        else material->SetTexture(manager.LoadTexture(metallic, TextureType::METALLIC), TextureType::METALLIC);

    if (!roughness.empty())
        if (manager.HasTexture(roughness)) material->SetTexture(manager.GetTexture(roughness), TextureType::ROUGHNESS);
        else material->SetTexture(manager.LoadTexture(roughness, TextureType::ROUGHNESS), TextureType::ROUGHNESS);

    if (!ambient.empty())
        if (manager.HasTexture(ambient)) material->SetTexture(manager.GetTexture(ambient), TextureType::AO);
        else material->SetTexture(manager.LoadTexture(ambient, TextureType::AO), TextureType::AO);

    material->SetAlbedoColor(HexToColor(albedoColor));
    material->SetNormalScale(normalScale);
    material->SetMetallicScale(metallicScale);
    material->SetRoughnessScale(roughnessScale);

    return material;
}

} // namespace Blainn