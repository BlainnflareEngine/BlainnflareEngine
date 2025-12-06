#include "Components/MeshComponent.h"
#include "Render/Device.h"

namespace Blainn
{
    MeshComponent::MeshComponent()
    {
        auto &device = Device::GetInstance();
        ObjectCB = eastl::make_unique<UploadBuffer<ObjectConstants>>(device.GetDevice2().Get(), 1u /*amount of meshes in model*/, TRUE);
    }

    void MeshComponent::UpdateMeshCB(ObjectConstants& objectCBData)
    {
        m_perObjectCBData = objectCBData;
        ObjectCB->CopyData(0, m_perObjectCBData);
    }
}