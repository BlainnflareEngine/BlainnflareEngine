//
// Created by WhoLeb on 04-Jan-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//
#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Renderer/DebugRendererSimple.h>
#include <EASTL/array.h>
#include <EASTL/vector.h>
#include <string_view>

#include "aliases.h"
#include "Device.h"
#include "Shader.h"
#include "VertexTypes.h"

namespace Blainn
{
    class RootSignature;

    class DebugRenderer : public JPH::DebugRendererSimple
    {
        using Super = JPH::DebugRendererSimple;
    public:
        struct LineSegment
        {
            Vec3 from{};
            Vec3 to{};
            Color color{};
        };

        struct CapsuleDrawRequest
        {
            Mat4 matrix{};
            float halfHeightOfCylinder = 0.0f;
            float radius = 0.0f;
            Color color{};
        };

        struct CylinderDrawRequest
        {
            Mat4 matrix{};
            float halfHeight = 0.0f;
            float radius = 0.0f;
            Color color{};
        };

        DebugRenderer(Device& device);
        virtual ~DebugRenderer();

        void BeginDebugRenderPass(ID3D12GraphicsCommandList2* commandList, const D3D12_CPU_DESCRIPTOR_HANDLE& rtvDescriptor, const
                                  D3D12_CPU_DESCRIPTOR_HANDLE& dsvDescriptor);
        void EndDebugRenderPass();

        void SetViewProjMatrix(const Mat4& viewProjMat);

        virtual void DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) override;
        virtual void DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor,
                                  ECastShadow inCastShadow) override;
        void DrawLine(const LineSegment &lineSegment);
        void DrawArrow(Vec3 inFrom, Vec3 inTo, Color color, float size);

        void DrawTriangle(Vec3 inV1, Vec3 inV2, Vec3 inV3, Color inColor);

        void DrawWireBox(Vec3 min, Vec3 max, Color color);
        void DrawWireBox(Mat4 matrix, Vec3 min, Vec3 max, Color color);

        void DrawWireSphere(Vec3 center, float radius, Color color);
        void DrawWireUnitSphere(Mat4 matrix, Color color);

        void DrawCapsule(const CapsuleDrawRequest &request);

        void DrawCylinder(const CylinderDrawRequest &request);

        void DrawLineList(const eastl::vector<VertexPositionColor>::iterator& first, const eastl::vector<VertexPositionColor>::iterator& last);

        // this is not working, we didn't yet bother to render text!
        virtual void DrawText3D([[maybe_unused]] JPH::RVec3Arg inPosition, [[maybe_unused]] const std::string_view &inString,
                                [[maybe_unused]] JPH::ColorArg inColor, [[maybe_unused]] float inHeight) override {}

        bool IsDebugEnabled() const {return m_bIsDebugEnabled;}
        void SetDebugEnabled(bool value) {m_bIsDebugEnabled = value;}
        void ClearDebugList() {m_lineListVertices.clear(); m_triangleListVertices.clear();}

    private:
        void CreateRootSignature();
        void CompileShaders();
        void CreatePSO();

        struct BufferCreateRequest
        {
            size_t index = 0u;
            size_t size = 0u;
        };
        ComPtr<ID3D12Resource> CreateBuffer(const BufferCreateRequest &request);

    private:
        DXGI_FORMAT BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        DXGI_FORMAT DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

        bool m_bIsRenderPassOngoing = false;
        bool m_bIsDebugEnabled = false;

        Device& m_device;

        eastl::shared_ptr<RootSignature> m_rootSignature;
        ComPtr<ID3DBlob> m_vertexShader;
        ComPtr<ID3DBlob> m_pixelShader;
        Microsoft::WRL::ComPtr<ID3D12PipelineState> m_debugLinePipelineState;
        Microsoft::WRL::ComPtr<ID3D12PipelineState> m_debugTrianglePipelineState;

        ID3D12GraphicsCommandList2* m_commandList;
        Mat4 m_viewProj;

        eastl::vector<VertexPositionColor> m_lineListVertices;
        eastl::vector<VertexPositionColor> m_triangleListVertices;

        uint64_t m_currentFrame = 0;
        eastl::array<ComPtr<ID3D12Resource>, 4> m_debugRequests;
    };
}
