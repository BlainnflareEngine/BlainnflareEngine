//
// Created by gorev on 30.09.2025.
//

#pragma once
#include "FileSystemObject.h"
#include "MeshData.h"

namespace Blainn
{

class Model : public FileSystemObject
{
public:
    Model(const Path &absolutPath);
    Model(const Model &other, const Path &absolutPath);
    Model(Model &&other, const Path &absolutPath) noexcept;
    ~Model() override;

    virtual void Copy() override;
    virtual void Delete() override;
    virtual void Move() override;

    eastl::vector<MeshData> &GetMeshes();
    void SetMeshes(const eastl::vector<MeshData> &meshes);

private:
    eastl::vector<MeshData> m_meshes;
};

} // namespace Blainn