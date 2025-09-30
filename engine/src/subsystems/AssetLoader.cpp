//
// Created by gorev on 30.09.2025.
//

#include "subsystems/AssetLoader.h"

#include "MeshData.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <filesystem>

namespace Blainn
{


void AssetLoader::Init()
{
    BF_INFO("AssetLoader Init");
}


void AssetLoader::Destroy()
{
    BF_INFO("AssetLoader Destroy");
}


Model AssetLoader::ImportModel(const Path &path)
{
    auto model = Model(path);

    if (path.empty())
    {
        BF_ERROR("AssetLoader ImportModel: path is empty");
    }

    Assimp::Importer importer;
    const aiScene *scene =
        importer.ReadFile(path.string(), aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_CalcTangentSpace
                                             | aiProcess_FixInfacingNormals | aiProcess_ConvertToLeftHanded);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        BF_ERROR("AssetLoader ImportModel: error loading model " + std::string(importer.GetErrorString()));
        return model;
    }

    ProcessNode(path, *scene->mRootNode, *scene, Mat4::Identity, model);

    return model;
}


void AssetLoader::ProcessNode(const std::filesystem::path &path, const aiNode &node, const aiScene &scene,
                              const Mat4 &parentMatrix, Model &model)
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


MeshData AssetLoader::ProcessMesh(const std::filesystem::path &path, const aiMesh &mesh, const aiScene &scene,
                                  const aiNode &node, const Mat4 &parentMatrix, Model &model)
{
    MeshData result_mesh = MeshData();
    result_mesh.parentMatrix = parentMatrix;

    for (size_t i = 0; i < mesh.mNumVertices; ++i)
    {
        Vertex vertex;

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

} // namespace Blainn