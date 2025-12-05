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
        m_meshHandle = meshHandle;

        if (material) m_materialHandle = material;
        else m_materialHandle = AssetManager::GetInstance().GetDefaultMaterialHandle();
    }

    MeshComponent(eastl::shared_ptr<MeshHandle> &&meshHandle, eastl::shared_ptr<MaterialHandle> &&material)
        : MeshComponent()
    {
        m_meshHandle = eastl::move(meshHandle);
        m_materialHandle = eastl::move(material);
    }

    void UpdateMeshCB(ObjectConstants &objectCBData);

    eastl::shared_ptr<MeshHandle> m_meshHandle;
    eastl::shared_ptr<MaterialHandle> m_materialHandle;

    eastl::unique_ptr<UploadBuffer<ObjectConstants>> ObjectCB = nullptr;

    ObjectConstants m_perObjectCBData;
};
} // namespace Blainn