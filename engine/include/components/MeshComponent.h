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

        InitializeCB();
    }

    MeshComponent(eastl::shared_ptr<MeshHandle> &&meshHandle, eastl::shared_ptr<MaterialHandle> &&material)
        : MeshComponent()
    {
        MeshHandle = eastl::move(meshHandle);
        MaterialHandle = eastl::move(material);

        InitializeCB();
    }

    MeshComponent(const MeshComponent &other)
        : MeshHandle(other.MeshHandle)
        , MaterialHandle(other.MaterialHandle)
        , PerObjectCBData(other.PerObjectCBData)
        , Enabled(other.Enabled)
        , IsWalkable(other.IsWalkable)
    {
        InitializeCB();
    }

    MeshComponent(MeshComponent &&other)
        : MeshHandle(eastl::move(other.MeshHandle))
        , MaterialHandle(eastl::move(other.MaterialHandle))
        , PerObjectCBData(eastl::move(other.PerObjectCBData))
        , Enabled(eastl::move(other.Enabled))
        , IsWalkable(eastl::move(other.IsWalkable))
    {
        InitializeCB();
    }

    MeshComponent &operator=(const MeshComponent &other)
    {
        if (this != &other)
        {
            MeshHandle = other.MeshHandle;
            MaterialHandle = other.MaterialHandle;
            PerObjectCBData = other.PerObjectCBData;
            Enabled = other.Enabled;
            IsWalkable = other.IsWalkable;
            InitializeCB();
        }
        return *this;
    }

    MeshComponent& operator=(MeshComponent&& other) noexcept
    {
        if (this != &other)
        {
            MeshHandle = eastl::move(other.MeshHandle);
            MaterialHandle = eastl::move(other.MaterialHandle);
            PerObjectCBData = eastl::move(other.PerObjectCBData);
            Enabled = other.Enabled;
            IsWalkable = other.IsWalkable;
            InitializeCB();
        }
        return *this;
    }

    void UpdateMeshCB(ObjectConstants &objectCBData);

    eastl::shared_ptr<MeshHandle> MeshHandle;
    eastl::shared_ptr<MaterialHandle> MaterialHandle;

    eastl::unique_ptr<UploadBuffer<ObjectConstants>> ObjectCB = nullptr;

    ObjectConstants PerObjectCBData;

    bool Enabled = true;
    // TODO: use layers in future
    bool IsWalkable = false;

private:
    void InitializeCB()
    {
        auto &device = Device::GetInstance();
        ObjectCB = eastl::make_unique<UploadBuffer<ObjectConstants>>(device.GetDevice2().Get(),
                                                                     1u /*amount of meshes in model*/, TRUE);
    }
};
} // namespace Blainn