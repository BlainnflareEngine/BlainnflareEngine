//
// Created by WhoLeb on 04-Jan-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#include "pch.h"
#include "Render/DebugRenderer.h"

#include <cmath>

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

    m_currentFrame++;
}

void DebugRenderer::EndDebugRenderPass()
{
    BLAINN_PROFILE_SCOPE_DYNAMIC("EndDebugRenderPass");

    if (m_lineListVertices.empty() && m_triangleListVertices.empty()) return;

    auto currentFrameBuffer = m_debugRequests[m_currentFrame % 4];
    const UINT lineVertexCount = static_cast<UINT>(m_lineListVertices.size());
    const UINT triangleVertexCount = static_cast<UINT>(m_triangleListVertices.size());
    const UINT totalVertexBufferSize = static_cast<UINT>(
        (m_lineListVertices.size() + m_triangleListVertices.size()) * sizeof(VertexPositionColor));

    if (currentFrameBuffer == nullptr || currentFrameBuffer.Get()->GetDesc().Width < totalVertexBufferSize)
        currentFrameBuffer = CreateBuffer({m_currentFrame % 4, totalVertexBufferSize});
    if (currentFrameBuffer == nullptr)
        return;


    UINT8 *pVertexDataBegin;
    CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.
    ThrowIfFailed(currentFrameBuffer->Map(0, &readRange, reinterpret_cast<void **>(&pVertexDataBegin)));
    if (!m_lineListVertices.empty())
    {
        memcpy(pVertexDataBegin, m_lineListVertices.data(), m_lineListVertices.size() * sizeof(VertexPositionColor));
    }
    if (!m_triangleListVertices.empty())
    {
        memcpy(pVertexDataBegin + m_lineListVertices.size() * sizeof(VertexPositionColor), m_triangleListVertices.data(),
               m_triangleListVertices.size() * sizeof(VertexPositionColor));
    }
    currentFrameBuffer->Unmap(0, nullptr);

    D3D12_VERTEX_BUFFER_VIEW vbView = {};
    vbView.BufferLocation = currentFrameBuffer->GetGPUVirtualAddress();
    vbView.StrideInBytes = sizeof(VertexPositionColor);
    vbView.SizeInBytes = totalVertexBufferSize;

    m_commandList->IASetVertexBuffers(0, 1, &vbView);
    if (lineVertexCount > 0u)
    {
        m_commandList->SetPipelineState(m_debugLinePipelineState.Get());
        m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
        m_commandList->DrawInstanced(lineVertexCount, 1, 0, 0);
    }
    if (triangleVertexCount > 0u)
    {
        m_commandList->SetPipelineState(m_debugTrianglePipelineState.Get());
        m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        m_commandList->DrawInstanced(triangleVertexCount, 1, lineVertexCount, 0);
    }

    size_t prevSize = m_lineListVertices.size();
    size_t prevTriangleSize = m_triangleListVertices.size();
    m_lineListVertices.clear();
    m_triangleListVertices.clear();
    m_lineListVertices.reserve(prevSize);
    m_triangleListVertices.reserve(prevTriangleSize);

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
    Color col = {static_cast<float>(inColor.r) / 255.0f, static_cast<float>(inColor.g) / 255.0f,
                 static_cast<float>(inColor.b) / 255.0f, static_cast<float>(inColor.a) / 255.0f};
    Vec3 from = {inFrom.GetX(), inFrom.GetY(), inFrom.GetZ()};
    Vec3 to = {inTo.GetX(), inTo.GetY(), inTo.GetZ()};
    DrawLine({from, to, col});
}

void Blainn::DebugRenderer::DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3,
                                         JPH::ColorArg inColor, ECastShadow inCastShadow)
{
    (void)inCastShadow;
    if (!m_bIsDebugEnabled)
        return;

    const Color col = {static_cast<float>(inColor.r) / 255.0f, static_cast<float>(inColor.g) / 255.0f,
                       static_cast<float>(inColor.b) / 255.0f, static_cast<float>(inColor.a) / 255.0f};
    const Vec3 V1 = {inV1.GetX(), inV1.GetY(), inV1.GetZ()};
    const Vec3 V2 = {inV2.GetX(), inV2.GetY(), inV2.GetZ()};
    const Vec3 V3 = {inV3.GetX(), inV3.GetY(), inV3.GetZ()};
    DrawTriangle(V1, V2, V3, col);
}

void Blainn::DebugRenderer::DrawLine(const LineSegment &lineSegment)
{
    BLAINN_PROFILE_SCOPE(DrawLine);
    if (!m_bIsDebugEnabled)
        return;
    m_lineListVertices.push_back({lineSegment.from, lineSegment.color});
    m_lineListVertices.push_back({lineSegment.to, lineSegment.color});
}

void DebugRenderer::DrawArrow(Vec3 inFrom, Vec3 inTo, Color color, float size)
{
    if (!m_bIsDebugEnabled)
        return;
    DrawLine({inFrom, inTo, color});

    if (size > 0.0f)
    {
        // Draw arrow head
        Vec3 dir = Vec3(inTo - inFrom);
        float len = dir.Length();
        if (len != 0.0f) dir = dir * (size / len);
        else dir = Vec3(size, 0, 0);
        Vec3 perp = size * GetNormalizedPerpendicular(dir);
        DrawLine({inTo - dir + perp, inTo, color});
        DrawLine({inTo - dir - perp, inTo, color});
    }
}

void DebugRenderer::DrawTriangle(Vec3 inV1, Vec3 inV2, Vec3 inV3, Color inColor)
{
    BLAINN_PROFILE_SCOPE(DrawTriangle);
    if (!m_bIsDebugEnabled)
        return;
    m_triangleListVertices.push_back({inV1, inColor});
    m_triangleListVertices.push_back({inV2, inColor});
    m_triangleListVertices.push_back({inV3, inColor});
}

void DebugRenderer::DrawWireBox(Vec3 min, Vec3 max, Color color)
{
    if (!m_bIsDebugEnabled)
        return;
    JPH::Vec3 vmin = {min.x, min.y, min.z};
    JPH::Vec3 vmax = {max.x, max.y, max.z};
    JPH::AABox box = {vmin, vmax};
    JPH::Color col;
    col.r = static_cast<JPH::uint8>(floor(color.R() * 255.0f));
    col.g = static_cast<JPH::uint8>(floor(color.G() * 255.0f));
    col.b = static_cast<JPH::uint8>(floor(color.B() * 255.0f));
    col.a = static_cast<JPH::uint8>(floor(color.A() * 255.0f));
    Super::DrawBox(box, col, ECastShadow::Off, EDrawMode::Wireframe);
}

void DebugRenderer::DrawWireBox(Mat4 matrix, Vec3 min, Vec3 max, Color color)
{
    if (!m_bIsDebugEnabled)
        return;
    JPH::Vec3 vmin = {min.x, min.y, min.z};
    JPH::Vec3 vmax = {max.x, max.y, max.z};
    JPH::AABox box = {vmin, vmax};
    JPH::Color col;
    col.r = static_cast<JPH::uint8>(floor(color.R() * 255.0f));
    col.g = static_cast<JPH::uint8>(floor(color.G() * 255.0f));
    col.b = static_cast<JPH::uint8>(floor(color.B() * 255.0f));
    col.a = static_cast<JPH::uint8>(floor(color.A() * 255.0f));

    JPH::Vec4 row1 = {matrix._11, matrix._12, matrix._13, matrix._14};
    JPH::Vec4 row2 = {matrix._21, matrix._22, matrix._23, matrix._24};
    JPH::Vec4 row3 = {matrix._31, matrix._32, matrix._33, matrix._34};
    JPH::Vec4 row4 = {matrix._41, matrix._42, matrix._43, matrix._44};

    JPH::Mat44 jphMat{row1, row2, row3, row4};

    Super::DrawBox(jphMat, box, col, ECastShadow::Off, EDrawMode::Wireframe);
}

void DebugRenderer::DrawWireSphere(Vec3 center, float radius, Color color)
{
    if (!m_bIsDebugEnabled)
        return;
    JPH::Vec3 V1 = {center.x, center.y, center.z};

    JPH::Color col;
    col.r = static_cast<JPH::uint8>(floor(color.R() * 255.0f));
    col.g = static_cast<JPH::uint8>(floor(color.G() * 255.0f));
    col.b = static_cast<JPH::uint8>(floor(color.B() * 255.0f));
    col.a = static_cast<JPH::uint8>(floor(color.A() * 255.0f));

    Super::DrawSphere(V1, radius, col, ECastShadow::Off, EDrawMode::Wireframe);
}

void DebugRenderer::DrawWireUnitSphere(Mat4 matrix, Color color)
{
    if (!m_bIsDebugEnabled)
        return;
    JPH::Color col;
    col.r = static_cast<JPH::uint8>(floor(color.R() * 255.0f));
    col.g = static_cast<JPH::uint8>(floor(color.G() * 255.0f));
    col.b = static_cast<JPH::uint8>(floor(color.B() * 255.0f));
    col.a = static_cast<JPH::uint8>(floor(color.A() * 255.0f));

    JPH::Vec4 row1 = {matrix._11, matrix._12, matrix._13, matrix._14};
    JPH::Vec4 row2 = {matrix._21, matrix._22, matrix._23, matrix._24};
    JPH::Vec4 row3 = {matrix._31, matrix._32, matrix._33, matrix._34};
    JPH::Vec4 row4 = {matrix._41, matrix._42, matrix._43, matrix._44};

    JPH::Mat44 jphMat{row1, row2, row3, row4};

    Super::DrawUnitSphere(jphMat, col, ECastShadow::Off, EDrawMode::Wireframe);
}

void DebugRenderer::DrawCapsule(const CapsuleDrawRequest &request)
{
    if (!m_bIsDebugEnabled)
        return;
    JPH::Color col;
    col.r = static_cast<JPH::uint8>(std::floor(request.color.R() * 255.0f));
    col.g = static_cast<JPH::uint8>(std::floor(request.color.G() * 255.0f));
    col.b = static_cast<JPH::uint8>(std::floor(request.color.B() * 255.0f));
    col.a = static_cast<JPH::uint8>(std::floor(request.color.A() * 255.0f));

    JPH::Vec4 row1 = {request.matrix._11, request.matrix._12, request.matrix._13, request.matrix._14};
    JPH::Vec4 row2 = {request.matrix._21, request.matrix._22, request.matrix._23, request.matrix._24};
    JPH::Vec4 row3 = {request.matrix._31, request.matrix._32, request.matrix._33, request.matrix._34};
    JPH::Vec4 row4 = {request.matrix._41, request.matrix._42, request.matrix._43, request.matrix._44};

    JPH::Mat44 jphMat{row1, row2, row3, row4};

    Super::DrawCapsule(jphMat, request.halfHeightOfCylinder, request.radius, col);
}

void DebugRenderer::DrawCylinder(const CylinderDrawRequest &request)
{
    if (!m_bIsDebugEnabled)
        return;
    JPH::Color col;
    col.r = static_cast<JPH::uint8>(std::floor(request.color.R() * 255.0f));
    col.g = static_cast<JPH::uint8>(std::floor(request.color.G() * 255.0f));
    col.b = static_cast<JPH::uint8>(std::floor(request.color.B() * 255.0f));
    col.a = static_cast<JPH::uint8>(std::floor(request.color.A() * 255.0f));

    JPH::Vec4 row1 = {request.matrix._11, request.matrix._12, request.matrix._13, request.matrix._14};
    JPH::Vec4 row2 = {request.matrix._21, request.matrix._22, request.matrix._23, request.matrix._24};
    JPH::Vec4 row3 = {request.matrix._31, request.matrix._32, request.matrix._33, request.matrix._34};
    JPH::Vec4 row4 = {request.matrix._41, request.matrix._42, request.matrix._43, request.matrix._44};

    JPH::Mat44 jphMat{row1, row2, row3, row4};

    Super::DrawCylinder(jphMat, request.halfHeight, request.radius, col);
}

void DebugRenderer::DrawLineList(const eastl::vector<VertexPositionColor>::iterator &first,
    const eastl::vector<VertexPositionColor>::iterator &last)
{
    if (!m_bIsDebugEnabled)
        return;
    m_lineListVertices.insert(m_lineListVertices.end(), first, last);
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
    D3D12_GRAPHICS_PIPELINE_STATE_DESC debugPSODesc;
    ZeroMemory(&debugPSODesc, sizeof(debugPSODesc));
    debugPSODesc.pRootSignature = m_rootSignature->Get();
    debugPSODesc.VS = D3D12_SHADER_BYTECODE(m_vertexShader->GetBufferPointer(), m_vertexShader->GetBufferSize());
    debugPSODesc.PS = D3D12_SHADER_BYTECODE(m_pixelShader->GetBufferPointer(), m_pixelShader->GetBufferSize());

    D3D12_RENDER_TARGET_BLEND_DESC RTBlendDesc;
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

    ThrowIfFailed(m_device.CreateGraphicsPipelineState(debugPSODesc, m_debugLinePipelineState));

    debugPSODesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    debugPSODesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
    ThrowIfFailed(m_device.CreateGraphicsPipelineState(debugPSODesc, m_debugTrianglePipelineState));
}

ComPtr<ID3D12Resource> DebugRenderer::CreateBuffer(const BufferCreateRequest &request)
{
    Microsoft::WRL::ComPtr<ID3D12Resource> lineVertexBuffer;

    const auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    const auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(request.size);
    if (auto hr = m_device.GetDevice2()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resDesc,
                                                                 D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                                 IID_PPV_ARGS(lineVertexBuffer.GetAddressOf()));
        FAILED(hr))
    {
        BF_ERROR("Could not create vertex buffer for debug rendering!");
        return nullptr;
    }
    m_debugRequests[request.index] = lineVertexBuffer;
    lineVertexBuffer->SetName(L"Debug Vertex Buffer");
    return lineVertexBuffer;
}
