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

    ResetTextureOffsetsTable();
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
                                                 | aiProcess_FindInvalidData | aiProcess_FixInfacingNormals | aiProcess_FlipUVs | aiProcess_ConvertToLeftHanded);

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
    auto cmdAlloc = cmdQueue->GetCommandAllocator();
    auto cmdList = cmdQueue->GetCommandList(cmdAlloc.Get());

    model.CreateGPUBuffers(cmdList.Get());

    cmdQueue->ExecuteCommandList(cmdList.Get());
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

        if (mesh.HasTextureCoords(0)) vertex.texCoord = GetTextCoords(mesh, i);

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

//void TextureComponent::LoadCubemap(ComPtr<ID3D12GraphicsCommandList2> commandList, const std::string &path)
//{
//    auto device = Device::GetInstance();
//    auto comDevice = device.GetDevice2();
//
//    ScratchImage image;
//    ThrowIfFailed(LoadFromDDSFile(std::wstring(path.begin(), path.end()).c_str(), DDS_FLAGS_FORCE_RGB, nullptr, image));
//
//    const TexMetadata &metadata = image.GetMetadata();
//
//    if (!(metadata.miscFlags & TEX_MISC_TEXTURECUBE))
//    {
//        printf("Ôàéë íå ÿâëÿåòñÿ êóáè÷åñêîé òåêñòóðîé (cubemap DDS)\n");
//        return;
//    }
//
//    ScratchImage mipChain;
//    if (metadata.mipLevels == 1)
//    {
//        ThrowIfFailed(GenerateMipMaps(*image.GetImages(), TEX_FILTER_DEFAULT, 0, mipChain));
//    }
//    else
//    {
//        mipChain = std::move(image);
//    }
//
//    const TexMetadata &mipMetadata = mipChain.GetMetadata();
//
//    D3D12_RESOURCE_DESC texDesc = {};
//    texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
//    texDesc.Alignment = 0;
//    texDesc.Width = static_cast<UINT>(mipMetadata.width);
//    texDesc.Height = static_cast<UINT>(mipMetadata.height);
//    texDesc.DepthOrArraySize = static_cast<UINT16>(mipMetadata.arraySize); // äîëæíî áûòü 6
//    texDesc.MipLevels = static_cast<UINT16>(mipMetadata.mipLevels);
//    texDesc.Format = mipMetadata.format;
//    texDesc.SampleDesc.Count = 1;
//    texDesc.SampleDesc.Quality = 0;
//    texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
//    texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
//
//    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
//
//    ThrowIfFailed(comDevice->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &texDesc,
//                                                  D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_Resource)));
//
//    std::vector<D3D12_SUBRESOURCE_DATA> subresources;
//    for (int face = 0; face < 6; ++face)
//    {
//        for (int mip = 0; mip < (int)metadata.mipLevels; ++mip)
//        {
//            const Image *img = mipChain.GetImage(mip, face, 0);
//            D3D12_SUBRESOURCE_DATA s = {};
//            s.pData = img->pixels;
//            s.RowPitch = img->rowPitch;
//            s.SlicePitch = img->slicePitch;
//            subresources.push_back(s);
//        }
//    }
//
//    UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_Resource.Get(), 0, static_cast<UINT>(subresources.size()));
//
//    CD3DX12_HEAP_PROPERTIES heapPropsUpload(D3D12_HEAP_TYPE_UPLOAD);
//    CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
//
//    ThrowIfFailed(comDevice->CreateCommittedResource(&heapPropsUpload, D3D12_HEAP_FLAG_NONE, &bufferDesc,
//                                                  D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
//                                                  IID_PPV_ARGS(&m_UploadBuffer)));
//
//    UpdateSubresources(commandList.Get(), m_Resource.Get(), m_UploadBuffer.Get(), (UINT64)0u, 0u, static_cast<UINT>(subresources.size()), subresources.data());
//
//    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_Resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
//
//    commandList->ResourceBarrier(1, &barrier);
//
//    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
//    srvDesc.Format = mipMetadata.format;
//    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
//    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
//    srvDesc.TextureCube.MipLevels = static_cast<UINT>(mipMetadata.mipLevels);
//    srvDesc.TextureCube.MostDetailedMip = 0;
//    srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
//
//    m_SRVHeapIndex = DescriptorHeaps::GetNextFreeIndex(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, GraphicAdapterPrimary);
//
//    CD3DX12_CPU_DESCRIPTOR_HANDLE handle(::GetCPUHandle(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, m_SRVHeapIndex, GraphicAdapterPrimary));
//
//    device->CreateShaderResourceView(m_Resource.Get(), &srvDesc, handle);
//
//    m_Initialized = true;
//    m_ResourcePath = path;
//}

eastl::shared_ptr<Texture> AssetLoader::LoadTexture(const Path &path, const TextureType type, uint32_t index)
{
    assert(path.is_relative());
    auto texture = eastl::make_shared<Texture>(path, type, index);
    Device::GetInstance().Flush();
    texture->DisposeUploaders();
    return texture;
}

void Blainn::AssetLoader::ResetTextureOffsetsTable()
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