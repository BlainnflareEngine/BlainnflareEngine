#include "Render/FreyaUtil.h"
#include "subsystems/Log.h"

ComPtr<ID3D12Resource> Blainn::FreyaUtil::CreateDefaultBuffer(const DefaultBufferParams &params)
{
    ComPtr<ID3D12Resource> defaultBuffer;

    if (params.device == nullptr || params.cmdList == nullptr || params.uploadBuffer == nullptr)
    {
        BF_ERROR("CreateDefaultBuffer called with invalid parameters.");
        return nullptr;
    }

    auto defaultHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    auto buffer = CD3DX12_RESOURCE_DESC::Buffer(params.byteSize);
    // Create the actual default buffer resource
    if (FAILED(params.device->CreateCommittedResource(
        &defaultHeap,
        D3D12_HEAP_FLAG_NONE,
        &buffer,
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(defaultBuffer.GetAddressOf()))))
    {
        BF_ERROR("Failed to create default buffer.");
        return nullptr;
    }


    auto uploadHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    // In order to copy CPU memory data into out default buffer, we need to create an intermediate upload heap
    if (FAILED(params.device->CreateCommittedResource(
        &uploadHeap,
        D3D12_HEAP_FLAG_NONE,
        &buffer,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(params.uploadBuffer->GetAddressOf()))))
    {
        BF_ERROR("Failed to create upload buffer.");
        return nullptr;
    }

    // Describe the data we want to copy into the default buffer.
    D3D12_SUBRESOURCE_DATA subResourceData = {};
    subResourceData.pData = params.initData;
    subResourceData.RowPitch = static_cast<LONG_PTR>(params.byteSize); // For buffers the size of the data we are copying in bytes.
    subResourceData.SlicePitch = subResourceData.RowPitch;  // For buffers the size of the data we are copying in bytes.

    auto transition = CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
    // Schedule to copy the data to the default buffer resource. At a high level, the helper function UpdateSubresources will copy the CPU memory
    // into the intermediate upload heap. Then, using ID3D12CommandList::CopySubresourceRegion, the intermediate upload heap data will be copied to mBuffer.
    params.cmdList->ResourceBarrier(1u, &transition);
    UpdateSubresources<1>(params.cmdList, defaultBuffer.Get(), params.uploadBuffer->Get(), 0, 0u, 1u, &subResourceData);
    transition = CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
    params.cmdList->ResourceBarrier(1u, &transition);

    // Note: uploadBuffer has to be kept alive after the above function calls because the command list has not been executed yet that performs the actual copy.
    // The caller can Release the uploadBuffer after it knows the copy has been executed.
    return defaultBuffer;
}

D3D12_GPU_VIRTUAL_ADDRESS Blainn::FreyaUtil::GetGPUVirtualAddress(const GPUVirtualAddressParams &params)
{
    return params.baseAddress + (UINT64)(params.byteStride * params.index);
}

ComPtr<ID3DBlob> Blainn::FreyaUtil::CompileShader(const std::wstring& fileName, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target)
{
#if defined(_DEBUG) | defined(DEBUG)
    // Enable better shader debugging with the graphics debugging tools.
    UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES;
#else
    UINT compileFlags = D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES;
#endif

    HRESULT hr = S_OK;

    ComPtr<ID3DBlob> byteCode = nullptr;
    ComPtr<ID3DBlob> errors;

    hr = D3DCompileFromFile(fileName.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        entrypoint.c_str(), target.c_str(), compileFlags, 0u, &byteCode, &errors);

    if (errors != nullptr)
        OutputDebugStringA((char*)errors->GetBufferPointer());

    ThrowIfFailed(hr);

    return byteCode;
}

UINT Blainn::FreyaUtil::CalcConstantBufferByteSize(const UINT byteSize)
{
    return (byteSize + D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1) & ~(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1);
}
