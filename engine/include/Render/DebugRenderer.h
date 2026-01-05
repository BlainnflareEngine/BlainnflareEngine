//
// Created by WhoLeb on 04-Jan-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//
#pragma once

#include "d3d12.h"
#include <wrl.h>

#include "aliases.h"
#include "Device.h"
#include "Shader.h"
#include "VertexTypes.h"

#include "Jolt/Renderer/DebugRendererSimple.h"

namespace Blainn
{
    class RootSignature;

    class DebugRenderer : public JPH::DebugRendererSimple
    {
    public:
        DebugRenderer(Device& device);
        virtual ~DebugRenderer();

        void BeginDebugRenderPass(ID3D12GraphicsCommandList2* commandList, const D3D12_CPU_DESCRIPTOR_HANDLE& rtvDescriptor, const
                                  D3D12_CPU_DESCRIPTOR_HANDLE& dsvDescriptor);
        void EndDebugRenderPass();

        void SetViewProjMatrix(const Mat4& viewProjMat);

        virtual void DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) override;
        void DrawLine(Vec3 inFrom, Vec3 inTo, Color color);

        virtual void DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, ECastShadow inCastShadow) override;
        void DrawTriangle(Vec3 inV1, Vec3 inV2, Vec3 inV3, Color inColor);

        // this is not working, we didn't yet bother to render text!
        virtual void DrawText3D(JPH::RVec3Arg inPosition, const std::string_view &inString, JPH::ColorArg inColor, float inHeight) override {}

    private:
        void CreateRootSignature();
        void CompileShaders();
        void CreatePSO();

    private:
        DXGI_FORMAT BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        DXGI_FORMAT DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

        bool m_bIsRenderPassOngoing = false;

        Device& m_device;

        eastl::shared_ptr<RootSignature> m_rootSignature;
        ComPtr<ID3DBlob> m_vertexShader;
        ComPtr<ID3DBlob> m_pixelShader;
        Microsoft::WRL::ComPtr<ID3D12PipelineState> m_debugPipelineState;

        ID3D12GraphicsCommandList2* m_commandList;
        Mat4 m_viewProj;

        eastl::vector<VertexPositionColor> m_lineListVertices;

        eastl::deque<eastl::pair<uint64_t, ComPtr<ID3D12Resource>>> m_debugRequests;
    };
}
