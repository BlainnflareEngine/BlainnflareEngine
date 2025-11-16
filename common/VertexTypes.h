//
// Created by gorev on 30.09.2025.
//

#pragma once

#include "aliases.h"
#include <d3d12.h>

using namespace DirectX;

namespace Blainn
{
    struct VertexPositionNormalTangentBitangentUV;
    struct VertexPosition;
    
    using BlainnVertex = VertexPositionNormalTangentBitangentUV;
    using SimpleVertex = VertexPosition;

    struct VertexPositionNormalTangentBitangentUV
    {
        VertexPositionNormalTangentBitangentUV(){}

        VertexPositionNormalTangentBitangentUV(
            float px, float py, float pz,
            float nx, float ny, float nz,
            float tx, float ty, float tz,
            float bx, float by, float bz,
            float u, float v)
            : position(px, py, pz)
            , normal(nx, ny, nz)
            , tangent(tx, ty, tz)
            , bitangent(bx, by, bz)
            , texCoord(u, v)
        {}

        VertexPositionNormalTangentBitangentUV(
            const Vec3 &position,
            const Vec3 &normal,
            const Vec3 &tangent,
            const Vec3 &bitangent,
            const Vec2 &texCoord)
            : position(position)
            , normal(normal)
            , tangent(tangent)
            , bitangent(bitangent)
            , texCoord(texCoord)
        {}

        Vec3 position = Vec3(0.0f, 0.0f, 0.0f);
        Vec3 normal = Vec3(0.0f, 0.0f, 0.0f);
        Vec3 tangent = Vec3(0.0f, 0.0f, 0.0f);
        Vec3 bitangent = Vec3(0.0f, 0.0f, 0.0f);
        Vec2 texCoord = Vec2(0.0f, 0.0f);

    private:
        static constexpr inline UINT InputElementCount = 5u;
        static constexpr inline const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount] =
        {
            { "POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, 0u, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0u },
            { "NORMAL", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, 12u, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0u },
            { "TANGENT", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, 24u, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0u },
            { "BITANGENT", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, 36u, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0u },
            { "TEXCOORD", 0u, DXGI_FORMAT_R32G32_FLOAT, 0u, 48u, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0u },
        };

    public:
        static constexpr inline D3D12_INPUT_LAYOUT_DESC InputLayout =
        {
            InputElements,
            InputElementCount
        };
    };

    struct VertexPosition
    {
        VertexPosition(){}

        VertexPosition(float px, float py, float pz)
            : position(px, py, pz)
        {}

        VertexPosition(const Vec3 &position)
            : position(position)
        {}

        Vec3 position = Vec3(0.0f, 0.0f, 0.0f);

    private:
        static constexpr inline UINT InputElementCount = 1u;
        static constexpr inline const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount] =
        {
            { "POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, 0u, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0u },
        };

    public:
        static constexpr inline D3D12_INPUT_LAYOUT_DESC InputLayout =
        {
            InputElements,
            InputElementCount
        };
    };

} // namespace Blainn