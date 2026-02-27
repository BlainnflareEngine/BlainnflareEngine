//
// Created by WhoLeb on 24-Feb-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#pragma once

#include <cstdint>

namespace Blainn
{
/// Fill mode

/// [D3D11_FILL_MODE]: https://msdn.microsoft.com/en-us/library/windows/desktop/ff476131(v=vs.85).aspx
/// [D3D12_FILL_MODE]: https://msdn.microsoft.com/en-us/library/windows/desktop/dn770366(v=vs.85).aspx
/// This enumeration determines the fill mode to use when rendering triangles and mirrors the
/// [D3D11_FILL_MODE][]/[D3D12_FILL_MODE][] enum. It is used by RasterizerStateDesc structure to define the fill mode.
enum FILL_MODE : uint8_t
{
    /// Undefined fill mode.
    FILL_MODE_UNDEFINED = 0,

    /// Rasterize triangles using wireframe fill. \n
    /// Direct3D counterpart: D3D11_FILL_WIREFRAME/D3D12_FILL_MODE_WIREFRAME. OpenGL counterpart: GL_LINE.
    FILL_MODE_WIREFRAME,

    /// Rasterize triangles using solid fill. \n
    /// Direct3D counterpart: D3D11_FILL_SOLID/D3D12_FILL_MODE_SOLID. OpenGL counterpart: GL_FILL.
    FILL_MODE_SOLID,

    /// Helper value that stores the total number of fill modes in the enumeration.
    FILL_MODE_NUM_MODES
};


/// Cull mode

/// [D3D11_CULL_MODE]: https://msdn.microsoft.com/en-us/library/windows/desktop/ff476108(v=vs.85).aspx
/// [D3D12_CULL_MODE]: https://msdn.microsoft.com/en-us/library/windows/desktop/dn770354(v=vs.85).aspx
/// This enumeration defines which triangles are not drawn during the rasterization and mirrors
/// [D3D11_CULL_MODE][]/[D3D12_CULL_MODE][] enum. It is used by RasterizerStateDesc structure to define the polygon cull mode.
enum CULL_MODE : uint8_t
{
    /// Undefined cull mode.
    CULL_MODE_UNDEFINED = 0,

    /// Draw all triangles. \n
    /// Direct3D counterpart: D3D11_CULL_NONE/D3D12_CULL_MODE_NONE. OpenGL counterpart: glDisable( GL_CULL_FACE ).
    CULL_MODE_NONE,

    /// Do not draw triangles that are front-facing. Front- and back-facing triangles are determined
    /// by the RasterizerStateDesc::FrontCounterClockwise member. \n
    /// Direct3D counterpart: D3D11_CULL_FRONT/D3D12_CULL_MODE_FRONT. OpenGL counterpart: GL_FRONT.
    CULL_MODE_FRONT,

    /// Do not draw triangles that are back-facing. Front- and back-facing triangles are determined
    /// by the RasterizerStateDesc::FrontCounterClockwise member. \n
    /// Direct3D counterpart: D3D11_CULL_BACK/D3D12_CULL_MODE_BACK. OpenGL counterpart: GL_BACK.
    CULL_MODE_BACK,

    /// Helper value that stores the total number of cull modes in the enumeration.
    CULL_MODE_NUM_MODES
};


/// Rasterizer state description

/// This structure describes the rasterizer state and is part of the GraphicsPipelineDesc.
struct RasterizerStateDesc
{
    /// Determines triangle fill mode, see FILL_MODE for details.

    /// Default value: FILL_MODE_SOLID.
    FILL_MODE FillMode              = FILL_MODE_SOLID;

    /// Determines triangle cull mode, see CULL_MODE for details.

    /// Default value: CULL_MODE_BACK.
    CULL_MODE CullMode              = CULL_MODE_BACK;

    /// Determines if a triangle is front- or back-facing.

    /// If this parameter is True, a triangle will be considered front-facing if
    /// its vertices are counter-clockwise
    /// on the render target and considered back-facing if they are clockwise.
    /// If this parameter is False, the opposite is true.
    /// Default value: False.
    bool      FrontCounterClockwise = false;

    /// Enable clipping against near and far clip planes.

    /// Default value: True.
    ///
    /// By default, polygon faces are clipped against the near and far planes of the view
    /// frustum. If depth clipping is disabled, the depth of the fragments that would be
    /// clipped is clamped to the near/far plane instead of discarding them.
    ///
    /// To check if the device supports depth clamping, use the DepthClamp device feature.
    /// If it is not supported, the value of this member must be True.
    bool      DepthClipEnable       = true;

    /// Enable scissor-rectangle culling. All pixels outside an active scissor rectangle are culled.

    /// Default value: false.
    bool      ScissorEnable         = false;

    /// Specifies whether to enable line antialiasing.

    /// Default value: false.
    bool      AntialiasedLineEnable = false;

    /// Constant value added to the depth of a given pixel.

    /// Default value: 0.
    int32_t     DepthBias           = 0;

    /// Maximum depth bias of a pixel.

    /// \warning Depth bias clamp is not available in OpenGL
    ///
    /// Default value: 0.
    float   DepthBiasClamp          = 0.f;

    /// Scalar that scales the given pixel's slope before adding to the pixel's depth.

    /// Default value: 0.
    float   SlopeScaledDepthBias    = 0.f;

    constexpr RasterizerStateDesc() noexcept {}

    constexpr RasterizerStateDesc(FILL_MODE _fillMode,
                                  CULL_MODE _cullMode,
                                  bool      _frontCounterClockwise = RasterizerStateDesc{}.FrontCounterClockwise,
                                  bool      _depthClipEnable       = RasterizerStateDesc{}.DepthClipEnable      ,
                                  bool      _scissorEnable         = RasterizerStateDesc{}.ScissorEnable        ,
                                  bool      _antialiasedLineEnable = RasterizerStateDesc{}.AntialiasedLineEnable,
                                  int32_t   _depthBias             = RasterizerStateDesc{}.DepthBias            ,
                                  float     _depthBiasClamp        = RasterizerStateDesc{}.DepthBiasClamp       ,
                                  float     _slopeScaledDepthBias  = RasterizerStateDesc{}.SlopeScaledDepthBias)noexcept
       : FillMode              {_fillMode             }
       , CullMode              {_cullMode             }
       , FrontCounterClockwise {_frontCounterClockwise}
       , DepthClipEnable       {_depthClipEnable      }
       , ScissorEnable         {_scissorEnable        }
       , AntialiasedLineEnable {_antialiasedLineEnable}
       , DepthBias             {_depthBias            }
       , DepthBiasClamp        {_depthBiasClamp       }
       , SlopeScaledDepthBias  {_slopeScaledDepthBias }
    {
    }

    /// Tests if two structures are equivalent
    constexpr bool operator == (const RasterizerStateDesc& _rhs)const
    {
        return  FillMode              == _rhs.FillMode              &&
                CullMode              == _rhs.CullMode              &&
                FrontCounterClockwise == _rhs.FrontCounterClockwise &&
                DepthBias             == _rhs.DepthBias             &&
                DepthBiasClamp        == _rhs.DepthBiasClamp        &&
                SlopeScaledDepthBias  == _rhs.SlopeScaledDepthBias  &&
                DepthClipEnable       == _rhs.DepthClipEnable       &&
                ScissorEnable         == _rhs.ScissorEnable         &&
                AntialiasedLineEnable == _rhs.AntialiasedLineEnable;
        static_assert(sizeof(*this) == 20, "Did you add new members to RasterizerStateDesc? Please handle them here.");
    }

    constexpr bool operator != (const RasterizerStateDesc& _rhs)const
    {
        return !(*this == _rhs);
    }
};

} // namespace Blainn

