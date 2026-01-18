#pragma once

#include "Handles/Handle.h"
#include "Render/FreyaCoreTypes.h"
#include "Render/UploadBuffer.h"

namespace Blainn
{
struct MeshComponent
{
    MeshComponent();

    MeshComponent(const eastl::shared_ptr<MeshHandle> &meshHandle,
                  const eastl::shared_ptr<MaterialHandle> &material = nullptr)
        : MeshComponent()
    {
        MeshHandle = meshHandle;

        if (material) MaterialHandle = material;
        else MaterialHandle = AssetManager::GetInstance().GetDefaultMaterialHandle();
    }

    MeshComponent(eastl::shared_ptr<MeshHandle> &&meshHandle, eastl::shared_ptr<MaterialHandle> &&material)
        : MeshComponent()
    {
        MeshHandle = eastl::move(meshHandle);
        MaterialHandle = eastl::move(material);
    }

    void UpdateMeshCB(ObjectConstants &objectCBData);

    eastl::shared_ptr<MeshHandle> MeshHandle;
    eastl::shared_ptr<MaterialHandle> MaterialHandle;

    eastl::unique_ptr<UploadBuffer<ObjectConstants>> ObjectCB = nullptr;

    ObjectConstants PerObjectCBData;

    // TODO: use layers in future 
    bool IsWalkable = false;
};
} // namespace Blainn