#include "Render/Renderer.h"
#include "RenderSubsystem.h"

Blainn::Renderer::Renderer(ID3D12Device *pDevice, uint32_t width, uint32_t height)
    : m_device(pDevice)
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
    ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                         IID_PPV_ARGS(&m_commandAllocators[m_currBackBuffer])));

    // Create the command list.
    ThrowIfFailed(m_device->CreateCommandList(0u /*Single GPU*/,D3D12_COMMAND_LIST_TYPE_DIRECT, 
                                          m_commandAllocators[m_currBackBuffer].Get() /*Must match the command list type*/,
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
    //RenderSubsystem::m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
}

void Blainn::Renderer::CreateRtvAndDsvDescriptorHeaps()
{

}