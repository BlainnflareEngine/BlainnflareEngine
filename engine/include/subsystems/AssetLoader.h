//
// Created by gorev on 30.09.2025.
//

#pragma once
#include "file-system/Model.h"
// #include <assimp/mesh.h>


namespace std::filesystem
{
class path;
}

struct aiScene;
struct aiNode;
struct Texture;
struct aiMesh;

namespace Blainn
{

class AssetLoader
{
    friend class AssetManager;

public:
    AssetLoader() = default;

    void Init();
    void Destroy();

    Model ImportModel(const std::filesystem::path &path);
    Texture LoadTexture(const std::filesystem::path &path);

private:
    AssetLoader(const AssetLoader &) = delete;
    AssetLoader &operator=(const AssetLoader &) = delete;
    AssetLoader(const AssetLoader &&) = delete;
    AssetLoader &operator=(const AssetLoader &&) = delete;

    void ProcessNode(const std::filesystem::path &path, const aiNode &node, const aiScene &scene,
                     const Mat4 &parentMatrix, Model &model);
    MeshData ProcessMesh(const std::filesystem::path &path, const aiMesh &mesh, const aiScene &scene,
                         const aiNode &node, const Mat4 &parentMatrix, Model &model);

    static Vec3 GetPosition(const aiMesh &mesh, const unsigned int meshIndex);
    static Vec3 GetNormal(const aiMesh &mesh, const unsigned int meshIndex);
    static Vec3 GetTangent(const aiMesh &mesh, const unsigned int meshIndex);
    static Vec3 GetBitangent(const aiMesh &mesh, const unsigned int meshIndex);
    static Vec2 GetTextCoords(const aiMesh &mesh, const unsigned int meshIndex);
};

} // namespace Blainn