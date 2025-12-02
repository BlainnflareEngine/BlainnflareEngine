#pragma once

#include "Handles/Handle.h"
#include "Render/FreyaCoreTypes.h"
#include "Render/UploadBuffer.h"

namespace Blainn
{
    struct MeshComponent
    {
        MeshComponent();

        MeshComponent(const eastl::shared_ptr<MeshHandle> &meshHandle)
            : MeshComponent()
        {
            m_meshHandle = meshHandle;
        }

        MeshComponent(eastl::shared_ptr<MeshHandle> &&meshHandle)
            : MeshComponent()
        {
            m_meshHandle = eastl::move(meshHandle);
        }

        void UpdateMeshCB(ObjectConstants &objectCBData);

        eastl::shared_ptr<MeshHandle> m_meshHandle;

        eastl::unique_ptr<UploadBuffer<ObjectConstants>> ObjectCB = nullptr;

        ObjectConstants m_perObjectCBData;
    };
} // namespace Blainn