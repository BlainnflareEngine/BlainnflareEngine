#pragma once 

#include "Render/DXHelpers.h"

using Microsoft::WRL::ComPtr;

namespace Blainn
{
    class FreyaUtil
    {
    public:
        static ComPtr<ID3D12Resource> CreateDefaultBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, const void* initData, UINT64 byteSize, ComPtr<ID3D12Resource>& uploadBuffer);
        
        static UINT CalcConstantBufferByteSize(const UINT byteSize);
        
        static ComPtr<ID3DBlob> CompileShader(const std::wstring& fileName, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target);
    };
}