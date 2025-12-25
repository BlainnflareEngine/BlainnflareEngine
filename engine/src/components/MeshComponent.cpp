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
        // Here we should iterate over all model meshes updating corresponding data
        PerObjectCBData = objectCBData;
        PerObjectCBData.MaterialIndex = MaterialHandle->GetIndex();
        ObjectCB->CopyData(0, PerObjectCBData);
    }
}