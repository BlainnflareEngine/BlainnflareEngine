
#include "Render/RootSignature.h"
#include "Render/Device.h"

namespace Blainn
{
    RootSignature::RootSignature(D3D_ROOT_SIGNATURE_VERSION rootSignatureVersion /*=D3D_ROOT_SIGNATURE_VERSION_1*/)
        : m_rootSignatureVersion(rootSignatureVersion)
    {
    }

    void RootSignature::Create(const eastl::shared_ptr<Device>& device, UINT numParameters, const CD3DX12_ROOT_PARAMETER *rootParams, D3D12_ROOT_SIGNATURE_FLAGS flags)
    {
        auto staticSamplers = GetStaticSamplers();

        // Root signature is an array of root parameters
        m_rootSignatureDesc.Init(numParameters, rootParams, (UINT)staticSamplers.size(), staticSamplers.data(), flags);

        ComPtr<ID3DBlob> signature = nullptr;
        ComPtr<ID3DBlob> error = nullptr;
        ThrowIfFailed(D3D12SerializeRootSignature(&m_rootSignatureDesc, m_rootSignatureVersion, &signature, &error));
        ThrowIfFailed(device->CreateRootSignature(0u, signature->GetBufferPointer(), signature->GetBufferSize(), m_rootSignature));
    }

    std::array<const CD3DX12_STATIC_SAMPLER_DESC, 5> RootSignature::GetStaticSamplers()
    {
        const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
            0u,                               // shaderRegister
            D3D12_FILTER_MIN_MAG_MIP_POINT,   // filter
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
            D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

        const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
            1u,                               // shaderRegister
            D3D12_FILTER_MIN_MAG_MIP_LINEAR,  // filter
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
            D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

        const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
            2u,                              // shaderRegister
            D3D12_FILTER_ANISOTROPIC,        // filter
            D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressU
            D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressV
            D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressW
            0.0f,                            // mipLODBias
            8u);                             // maxAnisotropy

        const CD3DX12_STATIC_SAMPLER_DESC shadowSampler(
            3u, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER,
            D3D12_TEXTURE_ADDRESS_MODE_BORDER, 0.0f, 16u);

        const CD3DX12_STATIC_SAMPLER_DESC shadowComparison(
            4u, D3D12_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_BORDER,
            D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER, 0.0f, 16u,
            D3D12_COMPARISON_FUNC_LESS_EQUAL, D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK);

        return {pointWrap, linearWrap, anisotropicWrap, shadowSampler, shadowComparison};
    }
}