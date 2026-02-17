#pragma once 
#include <string>
#include <wrl/client.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include "DirectXTK12/Src/d3dx12.h"

#include "Render/DXHelpers.h"

using Microsoft::WRL::ComPtr;

namespace Blainn
{
    class FreyaUtil
    {
    public:
        struct DefaultBufferParams
        {
            ID3D12Device *device = nullptr;
            ID3D12GraphicsCommandList *cmdList = nullptr;
            const void *initData = nullptr;
            UINT64 byteSize = 0u;
            ComPtr<ID3D12Resource> *uploadBuffer = nullptr;
        };

        struct GPUVirtualAddressParams
        {
            D3D12_GPU_VIRTUAL_ADDRESS baseAddress = 0u;
            UINT byteStride = 0u;
            UINT index = 0u;
        };

        static ComPtr<ID3D12Resource> CreateDefaultBuffer(const DefaultBufferParams &params);
        
        static D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress(const GPUVirtualAddressParams &params);

        static UINT CalcConstantBufferByteSize(const UINT byteSize);
        
        static ComPtr<ID3DBlob> CompileShader(const std::wstring& fileName, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target);
    };
}
