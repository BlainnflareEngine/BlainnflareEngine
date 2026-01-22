//
// Created by WhoLeb on 04-Jan-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#include "Render/DebugRenderer.h"

#include "VertexTypes.h"
#include "helpers.h"
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
                                         const D3D12_CPU_DESCRIPTOR_HANDLE &rtvDescriptor,
                                         const D3D12_CPU_DESCRIPTOR_HANDLE &dsvDescriptor)
{
    m_bIsRenderPassOngoing = true;
    m_commandList = commandList;

    m_commandList->OMSetRenderTargets(1, &rtvDescriptor, TRUE, &dsvDescriptor);
    m_commandList->SetGraphicsRootSignature(m_rootSignature->Get());
    m_commandList->SetPipelineState(m_debugPipelineState.Get());

    m_currentFrame++;
}

void DebugRenderer::EndDebugRenderPass()
{
    BLAINN_PROFILE_SCOPE(EndDebugRenderPass);

    while (!m_debugRequests.empty() && m_currentFrame > m_debugRequests.front().first)
        m_debugRequests.pop_front();

    if (m_lineListVertices.empty()) return;

    UINT vertexBufferSize = m_lineListVertices.size() * sizeof(VertexPositionColor);
    Microsoft::WRL::ComPtr<ID3D12Resource> lineVertexBuffer;

    const auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    const auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
    if (auto hr = m_device.GetDevice2()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resDesc,
                                                                 D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                                 IID_PPV_ARGS(lineVertexBuffer.GetAddressOf()));
        FAILED(hr))
    {
        BF_ERROR("Could not create vertex buffer for debug rendering!");
        return;
    }
    m_debugRequests.push_back({m_currentFrame + 3, lineVertexBuffer});
    lineVertexBuffer->SetName(L"Debug Vertex Buffer");

    UINT8 *pVertexDataBegin;
    CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.
    ThrowIfFailed(lineVertexBuffer->Map(0, &readRange, reinterpret_cast<void **>(&pVertexDataBegin)));
    memcpy(pVertexDataBegin, m_lineListVertices.data(), vertexBufferSize);
    lineVertexBuffer->Unmap(0, nullptr);

    D3D12_VERTEX_BUFFER_VIEW vbView = {};
    vbView.BufferLocation = lineVertexBuffer->GetGPUVirtualAddress();
    vbView.StrideInBytes = sizeof(VertexPositionColor);
    vbView.SizeInBytes = vertexBufferSize;

    m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

    m_commandList->IASetVertexBuffers(0, 1, &vbView);
    m_commandList->DrawInstanced(m_lineListVertices.size(), 1, 0, 0);

    m_lineListVertices.clear();

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
    if (!m_bIsDebugEnabled)
        return;
    Color col = {inColor.r / 255.f, inColor.g / 255.f, inColor.b / 255.f, inColor.a / 255.f};
    Vec3 from = {inFrom.GetX(), inFrom.GetY(), inFrom.GetZ()};
    Vec3 to = {inTo.GetX(), inTo.GetY(), inTo.GetZ()};
    DrawLine(from, to, col);
}

void Blainn::DebugRenderer::DrawLine(Vec3 inFrom, Vec3 inTo, Color color)
{
    BLAINN_PROFILE_SCOPE(DrawLine);
    if (!m_bIsDebugEnabled)
        return;
    m_lineListVertices.push_back({inFrom, color});
    m_lineListVertices.push_back({inTo, color});
}

void DebugRenderer::DrawArrow(Vec3 inFrom, Vec3 inTo, Color color, float size)
{
    if (!m_bIsDebugEnabled)
        return;
    DrawLine(inFrom, inTo, color);

    if (size > 0.0f)
    {
        // Draw arrow head
        Vec3 dir = Vec3(inTo - inFrom);
        float len = dir.Length();
        if (len != 0.0f) dir = dir * (size / len);
        else dir = Vec3(size, 0, 0);
        Vec3 perp = size * GetNormalizedPerpendicular(dir);
        DrawLine(inTo - dir + perp, inTo, color);
        DrawLine(inTo - dir - perp, inTo, color);
    }
}

void DebugRenderer::DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor,
                                 ECastShadow inCastShadow)
{
    if (!m_bIsDebugEnabled)
        return;
    // DebugRendererSimple::DrawTriangle(inV1, inV2, inV3, inColor, inCastShadow);
    const Color col = {inColor.r / 255.f, inColor.g / 255.f, inColor.b / 255.f, inColor.a / 255.f};
    const Vec3 V1 = {inV1.GetX(), inV1.GetY(), inV1.GetZ()};
    const Vec3 V2 = {inV2.GetX(), inV2.GetY(), inV2.GetZ()};
    const Vec3 V3 = {inV3.GetX(), inV3.GetY(), inV3.GetZ()};

    DrawTriangle(V1, V2, V3, col);
}

void DebugRenderer::DrawTriangle(Vec3 inV1, Vec3 inV2, Vec3 inV3, Color inColor)
{
    BLAINN_PROFILE_SCOPE(DrawTriangle);
    if (!m_bIsDebugEnabled)
        return;
    VertexPositionColor lineVertices[] = {{inV1, inColor}, {inV2, inColor}, {inV3, inColor}, {inV1, inColor}};
    m_lineListVertices.push_back({inV1, inColor});
    m_lineListVertices.push_back({inV2, inColor});

    m_lineListVertices.push_back({inV2, inColor});
    m_lineListVertices.push_back({inV3, inColor});


    m_lineListVertices.push_back({inV3, inColor});
    m_lineListVertices.push_back({inV1, inColor});
}

void DebugRenderer::DrawWireBox(Vec3 min, Vec3 max, Color color)
{
    if (!m_bIsDebugEnabled)
        return;
    JPH::Vec3 vmin = {min.x, min.y, min.z};
    JPH::Vec3 vmax = {max.x, max.y, max.z};
    JPH::AABox box = {vmin, vmax};
    JPH::Color col;
    col.r = floor(color.R() * 255);
    col.g = floor(color.G() * 255);
    col.b = floor(color.B() * 255);
    col.a = floor(color.A() * 255);
    Super::DrawWireBox(box, col);
}

void DebugRenderer::DrawWireBox(Mat4 matrix, Vec3 min, Vec3 max, Color color)
{
    if (!m_bIsDebugEnabled)
        return;
    JPH::Vec3 vmin = {min.x, min.y, min.z};
    JPH::Vec3 vmax = {max.x, max.y, max.z};
    JPH::AABox box = {vmin, vmax};
    JPH::Color col;
    col.r = floor(color.R() * 255);
    col.g = floor(color.G() * 255);
    col.b = floor(color.B() * 255);
    col.a = floor(color.A() * 255);

    JPH::Vec4 row1 = {matrix._11, matrix._12, matrix._13, matrix._14};
    JPH::Vec4 row2 = {matrix._21, matrix._22, matrix._23, matrix._24};
    JPH::Vec4 row3 = {matrix._31, matrix._32, matrix._33, matrix._34};
    JPH::Vec4 row4 = {matrix._41, matrix._42, matrix._43, matrix._44};

    JPH::Mat44 jphMat{row1, row2, row3, row4};

    Super::DrawWireBox(jphMat, box, col);
}

void DebugRenderer::DrawWireSphere(Vec3 center, float radius, Color color)
{
    if (!m_bIsDebugEnabled)
        return;
    JPH::Vec3 V1 = {center.x, center.y, center.z};

    JPH::Color col;
    col.r = floor(color.R() * 255);
    col.g = floor(color.G() * 255);
    col.b = floor(color.B() * 255);
    col.a = floor(color.A() * 255);

    Super::DrawWireSphere(V1, radius, col);
}

void DebugRenderer::DrawWireUnitSphere(Mat4 matrix, Color color)
{
    if (!m_bIsDebugEnabled)
        return;
    JPH::Color col;
    col.r = floor(color.R() * 255);
    col.g = floor(color.G() * 255);
    col.b = floor(color.B() * 255);
    col.a = floor(color.A() * 255);

    JPH::Vec4 row1 = {matrix._11, matrix._12, matrix._13, matrix._14};
    JPH::Vec4 row2 = {matrix._21, matrix._22, matrix._23, matrix._24};
    JPH::Vec4 row3 = {matrix._31, matrix._32, matrix._33, matrix._34};
    JPH::Vec4 row4 = {matrix._41, matrix._42, matrix._43, matrix._44};

    JPH::Mat44 jphMat{row1, row2, row3, row4};

    Super::DrawWireUnitSphere(jphMat, col);
}

void DebugRenderer::DrawCapsule(Mat4 matrix, float halfHeightOfCylinder, float radius, Color color)
{
    if (!m_bIsDebugEnabled)
        return;
    JPH::Color col;
    col.r = floor(color.R() * 255);
    col.g = floor(color.G() * 255);
    col.b = floor(color.B() * 255);
    col.a = floor(color.A() * 255);

    JPH::Vec4 row1 = {matrix._11, matrix._12, matrix._13, matrix._14};
    JPH::Vec4 row2 = {matrix._21, matrix._22, matrix._23, matrix._24};
    JPH::Vec4 row3 = {matrix._31, matrix._32, matrix._33, matrix._34};
    JPH::Vec4 row4 = {matrix._41, matrix._42, matrix._43, matrix._44};

    JPH::Mat44 jphMat{row1, row2, row3, row4};

    Super::DrawCapsule(jphMat, halfHeightOfCylinder, radius, col);
}

void DebugRenderer::DrawCylinder(Mat4 matrix, float halfHeight, float radius, Color color)
{
    if (!m_bIsDebugEnabled)
        return;
    JPH::Color col;
    col.r = floor(color.R() * 255);
    col.g = floor(color.G() * 255);
    col.b = floor(color.B() * 255);
    col.a = floor(color.A() * 255);

    JPH::Vec4 row1 = {matrix._11, matrix._12, matrix._13, matrix._14};
    JPH::Vec4 row2 = {matrix._21, matrix._22, matrix._23, matrix._24};
    JPH::Vec4 row3 = {matrix._31, matrix._32, matrix._33, matrix._34};
    JPH::Vec4 row4 = {matrix._41, matrix._42, matrix._43, matrix._44};

    JPH::Mat44 jphMat{row1, row2, row3, row4};

    Super::DrawCylinder(jphMat, halfHeight, radius, col);
}

void Blainn::DebugRenderer::CreateRootSignature()
{
    m_rootSignature = eastl::make_shared<RootSignature>();

    // sorry for magic values, i don't want to drag the common header and the common types ;)
    CD3DX12_ROOT_PARAMETER slotRootParameter[1];
    slotRootParameter[0].InitAsConstants(sizeof(Mat4) / sizeof(float), SHADER_REGISTER(0u), REGISTER_SPACE_0,
                                         D3D12_SHADER_VISIBILITY_ALL);

    m_rootSignature->Create(m_device, ARRAYSIZE(slotRootParameter), slotRootParameter,
                            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
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

    debugPSODesc.InputLayout = VertexPositionColor::InputLayout;
    debugPSODesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    debugPSODesc.NumRenderTargets = 1u;
    debugPSODesc.RTVFormats[0] = BackBufferFormat;
    debugPSODesc.DSVFormat = DepthStencilFormat;
    debugPSODesc.SampleDesc = {1u, 0u};

    ThrowIfFailed(m_device.CreateGraphicsPipelineState(debugPSODesc, m_debugPipelineState));
}
