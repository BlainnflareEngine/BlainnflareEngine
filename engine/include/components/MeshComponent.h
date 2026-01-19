#pragma once

#include "Handles/Handle.h"
#include "Render/Device.h"
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

    MeshComponent(const MeshComponent &other)
        : MeshHandle(other.MeshHandle)
        , MaterialHandle(other.MaterialHandle)
        , PerObjectCBData(other.PerObjectCBData)
        , IsWalkable(other.IsWalkable)
    {
        auto &device = Device::GetInstance();
        ObjectCB = eastl::make_unique<UploadBuffer<ObjectConstants>>(device.GetDevice2().Get(), 1u /*amount of meshes in model*/, TRUE);
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