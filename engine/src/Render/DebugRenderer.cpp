//
// Created by WhoLeb on 04-Jan-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#include "Render/DebugRenderer.h"

#include "VertexTypes.h"
#include "Render/CommandQueue.h"
#include "Render/FreyaUtil.h"
#include "Render/RootSignature.h"

using namespace Blainn;

Blainn::DebugRenderer::DebugRenderer(Device &device)
    : m_device(device)
{
    JPH::DebugRendererSimple::Initialize();

    CreateRootSignature();
    CompileShaders();
    CreatePSO();
}

Blainn::DebugRenderer::~DebugRenderer()
{
}

void DebugRenderer::BeginDebugRenderPass(ID3D12GraphicsCommandList2 *commandList,
                                            const D3D12_CPU_DESCRIPTOR_HANDLE& rtvDescriptor,
                                            const D3D12_CPU_DESCRIPTOR_HANDLE& dsvDescriptor)
{
    m_bIsRenderPassOngoing = true;
    m_commandList = commandList;

    m_commandList->OMSetRenderTargets(1, &rtvDescriptor, TRUE, &dsvDescriptor);
    m_commandList->SetGraphicsRootSignature(m_rootSignature->Get());
    m_commandList->SetPipelineState(m_debugPipelineState.Get());
}

void DebugRenderer::EndDebugRenderPass()
{
    auto nextFence = m_device.GetCommandQueue()->Signal() + 1;
    for (auto it = m_lineRequests.end()--; it != m_lineRequests.begin() && it->first != -1; --it)
        it->first = nextFence;
    while (!m_lineRequests.empty() && m_device.GetCommandQueue()->IsFenceComplete(m_lineRequests.front().first))
        m_lineRequests.pop_front();

    m_bIsRenderPassOngoing = false;
}

void DebugRenderer::SetViewProjMatrix(const Mat4 &viewProjMat)
{
    if (!m_bIsRenderPassOngoing)
    {
        BF_ERROR("Trying render debug without initialization");
        return;
    }
    m_viewProj = viewProjMat;
    m_commandList->SetGraphicsRoot32BitConstants(0, sizeof(Mat4) / sizeof(float), &m_viewProj, 0);
}

void Blainn::DebugRenderer::DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor)
{
    Color col = {inColor.r / 255.f, inColor.g / 255.f, inColor.b / 255.f, inColor.a / 255.f};
    Vec3 from = {inFrom.GetX(), inFrom.GetY(), inFrom.GetZ()};
    Vec3 to = {inTo.GetX(), inTo.GetY(), inTo.GetZ()};
    DrawLine(from, to, col);
}

void Blainn::DebugRenderer::DrawLine(Vec3 inFrom, Vec3 inTo, Color color)
{
    if (!m_bIsRenderPassOngoing)
    {
        BF_ERROR("Trying render debug without initialization");
        return;
    }
    VertexPosition lineVertices[] = { inFrom, inTo };
    UINT vertexBufferSize = sizeof(lineVertices);
    Microsoft::WRL::ComPtr<ID3D12Resource> lineVertexBuffer;

    const auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    const auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
    ThrowIfFailed(m_device.GetDevice2()->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &resDesc,
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(lineVertexBuffer.GetAddressOf())
        ));
    m_lineRequests.push_back({-1, lineVertexBuffer});

    UINT8* pVertexDataBegin;
    CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
    ThrowIfFailed(lineVertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
    memcpy(pVertexDataBegin, lineVertices, sizeof(lineVertices));
    lineVertexBuffer->Unmap(0, nullptr);

    D3D12_VERTEX_BUFFER_VIEW vbView = {};
    vbView.BufferLocation = lineVertexBuffer->GetGPUVirtualAddress();
    vbView.StrideInBytes = sizeof(VertexPosition);
    vbView.SizeInBytes = vertexBufferSize;

    m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

    m_commandList->SetGraphicsRoot32BitConstants(1, sizeof(Vec4) / sizeof(float), &color, 0);

    m_commandList->IASetVertexBuffers(0, 1, &vbView);
    m_commandList->DrawInstanced(2, 1, 0, 0);


}

void Blainn::DebugRenderer::CreateRootSignature()
{
    m_rootSignature = eastl::make_shared<RootSignature>();

    // sorry for magic values, i don't want to drag the common header and the common types ;)
    CD3DX12_ROOT_PARAMETER slotRootParameter[2];
    slotRootParameter[0].InitAsConstants(sizeof(Mat4) / sizeof(float), SHADER_REGISTER(0u), REGISTER_SPACE_0, D3D12_SHADER_VISIBILITY_ALL);
    slotRootParameter[1].InitAsConstants(sizeof(Vec4) / sizeof(float), SHADER_REGISTER(1), REGISTER_SPACE_0, D3D12_SHADER_VISIBILITY_ALL);

    m_rootSignature->Create(m_device, ARRAYSIZE(slotRootParameter), slotRootParameter, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
}

void Blainn::DebugRenderer::CompileShaders()
{
    m_vertexShader = FreyaUtil::CompileShader(L"./Content/Shaders/DebugRender.hlsl", nullptr, "DebugVS", "vs_5_1");
    m_pixelShader = FreyaUtil::CompileShader(L"./Content/Shaders/DebugRender.hlsl", nullptr, "DebugPS", "ps_5_1");
}

void Blainn::DebugRenderer::CreatePSO()
{
    D3D12_GRAPHICS_PIPELINE_STATE_DESC debugPSODesc = {};
    debugPSODesc.pRootSignature = m_rootSignature->Get();
    debugPSODesc.VS = D3D12_SHADER_BYTECODE(m_vertexShader->GetBufferPointer(), m_vertexShader->GetBufferSize());
    debugPSODesc.PS = D3D12_SHADER_BYTECODE(m_pixelShader->GetBufferPointer(), m_pixelShader->GetBufferSize());

    D3D12_RENDER_TARGET_BLEND_DESC RTBlendDesc = {};
    ZeroMemory(&RTBlendDesc, sizeof(D3D12_RENDER_TARGET_BLEND_DESC));
    RTBlendDesc.BlendEnable = TRUE;
    RTBlendDesc.LogicOpEnable = FALSE;
    RTBlendDesc.SrcBlend = D3D12_BLEND_ONE;
    RTBlendDesc.DestBlend = D3D12_BLEND_ONE;
    RTBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
    RTBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
    RTBlendDesc.DestBlendAlpha = D3D12_BLEND_ONE;
    RTBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
    RTBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
    RTBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    debugPSODesc.BlendState.AlphaToCoverageEnable = FALSE;
    debugPSODesc.BlendState.IndependentBlendEnable = FALSE;
    debugPSODesc.BlendState.RenderTarget[0] = RTBlendDesc;
    debugPSODesc.SampleMask = UINT_MAX;

    debugPSODesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    debugPSODesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    debugPSODesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;

    debugPSODesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    debugPSODesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
    debugPSODesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

    debugPSODesc.InputLayout = VertexPosition::InputLayout;
    debugPSODesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    debugPSODesc.NumRenderTargets = 1u;
    debugPSODesc.RTVFormats[0] = BackBufferFormat;
    debugPSODesc.DSVFormat = DepthStencilFormat;
    debugPSODesc.SampleDesc = {1u, 0u};

    ThrowIfFailed(m_device.CreateGraphicsPipelineState(debugPSODesc, m_debugPipelineState));
}


