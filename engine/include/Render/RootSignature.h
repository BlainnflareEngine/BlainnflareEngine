#pragma once
#include <array>
#include <d3d12.h>
#include <DirectXTK12/Src/d3dx12.h>

#include "Render/DXHelpers.h"

namespace Blainn
{
    class Device;

    class RootSignature
    {
    public:
        enum ERootParam : UINT
        {
            PerObjectDataCB = 0,
            PerPassDataCB,
            MaterialsDataSB,
            PointLightsDataSB,
            SpotLightsDataSB,
            CascadedShadowMaps,
            GBufferTextures,
            SkyBox,
            Textures,

            NumRootParameters = 9u
        };

    public:
        RootSignature(D3D_ROOT_SIGNATURE_VERSION rootSignatureVersion = D3D_ROOT_SIGNATURE_VERSION_1);

        /*RootSignature(const RootSignature& lhs) = delete;
        RootSignature& operator=(const RootSignature& lhs) = delete;
        RootSignature(RootSignature&& rhs) = delete;
        RootSignature& operator=(RootSignature&& rhs) = delete;*/

    public:
        void Create(Device& device, UINT numParameters, const CD3DX12_ROOT_PARAMETER *rootParams, D3D12_ROOT_SIGNATURE_FLAGS flags);

        FORCEINLINE ID3D12RootSignature *Get() const
        {
            return m_rootSignature.Get();
        }

        FORCEINLINE const CD3DX12_ROOT_SIGNATURE_DESC &GetDesc() const
        {
            return m_rootSignatureDesc;
        }

        std::array<const CD3DX12_STATIC_SAMPLER_DESC, 5> GetStaticSamplers();

    private:
        ComPtr<ID3D12RootSignature> m_rootSignature;
        CD3DX12_ROOT_SIGNATURE_DESC m_rootSignatureDesc;
        D3D_ROOT_SIGNATURE_VERSION m_rootSignatureVersion;
    };
}
