#include "Render/Renderer.h"

Blainn::Renderer::Renderer(eastl::shared_ptr<Device> device, uint32_t width, uint32_t height)
    : m_device(device)
    , m_width(width)
    , m_height(height)
{
}

Blainn::Renderer::~Renderer()
{
}

void Blainn::Renderer::Init()
{    
    CreateCommandObjects();
}

void Blainn::Renderer::CreateCommandObjects()
{
    ThrowIfFailed(GetDevicePtr()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                         IID_PPV_ARGS(&m_commandAllocators[m_frameIndex])));

    // Create the command list.
    ThrowIfFailed(
        GetDevicePtr()->CreateCommandList(0u /*Single GPU*/, D3D12_COMMAND_LIST_TYPE_DIRECT,
                                          m_commandAllocators[m_frameIndex].Get() /*Must match the command list type*/,
                                          nullptr, IID_PPV_ARGS(&m_commandList)));

    // Command lists are created in the recording state, but there is nothing
    // to record yet. The main loop expects it to be closed, so close it now.
    ThrowIfFailed(m_commandList->Close());
}

void Blainn::Renderer::Update(float deltaTime)
{
    
}

void Blainn::Renderer::RenderScene(void)
{

}

void Blainn::Renderer::PopulateCommandList()
{

}

void Blainn::Renderer::ExecuteCommandLists()
{
    ID3D12CommandList *ppCommandLists[] = { m_commandList.Get() };
    GetRenderCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
}

void Blainn::Renderer::CreateRtvAndDsvDescriptorHeaps()
{

}

void Blainn::Renderer::CreateRootSignature()
{
}

void Blainn::Renderer::CreatePipelineStateObjects()
{
}

void Blainn::Renderer::CreateShaders()
{

}