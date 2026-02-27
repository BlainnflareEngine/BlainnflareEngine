//
// Created by WhoLeb on 24-Feb-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#pragma once
#include <cstdint>

#include "helpers.h"
#include "Render/Constants.h"

namespace Blainn
{
/// [D3D11_BLEND]: https://msdn.microsoft.com/en-us/library/windows/desktop/ff476086(v=vs.85).aspx
/// [D3D12_BLEND]: https://msdn.microsoft.com/en-us/library/windows/desktop/dn770338(v=vs.85).aspx
/// [glBlendFuncSeparate]: https://www.opengl.org/wiki/GLAPI/glBlendFuncSeparate
/// This enumeration defines blend factors for alpha-blending.
/// It generally mirrors [D3D11_BLEND][] and [D3D12_BLEND][] enumerations and is used by RenderTargetBlendDesc structure
/// to define source and destination blend factors for color and alpha channels.
/// \sa [D3D11_BLEND on MSDN][D3D11_BLEND], [D3D12_BLEND on MSDN][D3D12_BLEND], [glBlendFuncSeparate on OpenGL.org][glBlendFuncSeparate]
///
enum BLEND_FACTOR : uint8_t
{
    /// Undefined blend factor
    BLEND_FACTOR_UNDEFINED = 0,

    /// The blend factor is zero.\n
    /// Direct3D counterpart: D3D11_BLEND_ZERO/D3D12_BLEND_ZERO. OpenGL counterpart: GL_ZERO.
    BLEND_FACTOR_ZERO,

    /// The blend factor is one.\n
    /// Direct3D counterpart: D3D11_BLEND_ONE/D3D12_BLEND_ONE. OpenGL counterpart: GL_ONE.
    BLEND_FACTOR_ONE,

    /// The blend factor is RGB data from a pixel shader.\n
    /// Direct3D counterpart: D3D11_BLEND_SRC_COLOR/D3D12_BLEND_SRC_COLOR. OpenGL counterpart: GL_SRC_COLOR.
    BLEND_FACTOR_SRC_COLOR,

    /// The blend factor is 1-RGB, where RGB is the data from a pixel shader.\n
    /// Direct3D counterpart: D3D11_BLEND_INV_SRC_COLOR/D3D12_BLEND_INV_SRC_COLOR. OpenGL counterpart: GL_ONE_MINUS_SRC_COLOR.
    BLEND_FACTOR_INV_SRC_COLOR,

    /// The blend factor is alpha (A) data from a pixel shader.\n
    /// Direct3D counterpart: D3D11_BLEND_SRC_ALPHA/D3D12_BLEND_SRC_ALPHA. OpenGL counterpart: GL_SRC_ALPHA.
    BLEND_FACTOR_SRC_ALPHA,

    /// The blend factor is 1-A, where A is alpha data from a pixel shader.\n
    /// Direct3D counterpart: D3D11_BLEND_INV_SRC_ALPHA/D3D12_BLEND_INV_SRC_ALPHA. OpenGL counterpart: GL_ONE_MINUS_SRC_ALPHA.
    BLEND_FACTOR_INV_SRC_ALPHA,

    /// The blend factor is alpha (A) data from a render target.\n
    /// Direct3D counterpart: D3D11_BLEND_DEST_ALPHA/D3D12_BLEND_DEST_ALPHA. OpenGL counterpart: GL_DST_ALPHA.
    BLEND_FACTOR_DEST_ALPHA,

    /// The blend factor is 1-A, where A is alpha data from a render target.\n
    /// Direct3D counterpart: D3D11_BLEND_INV_DEST_ALPHA/D3D12_BLEND_INV_DEST_ALPHA. OpenGL counterpart: GL_ONE_MINUS_DST_ALPHA.
    BLEND_FACTOR_INV_DEST_ALPHA,

    /// The blend factor is RGB data from a render target.\n
    /// Direct3D counterpart: D3D11_BLEND_DEST_COLOR/D3D12_BLEND_DEST_COLOR. OpenGL counterpart: GL_DST_COLOR.
    BLEND_FACTOR_DEST_COLOR,

    /// The blend factor is 1-RGB, where RGB is the data from a render target.\n
    /// Direct3D counterpart: D3D11_BLEND_INV_DEST_COLOR/D3D12_BLEND_INV_DEST_COLOR. OpenGL counterpart: GL_ONE_MINUS_DST_COLOR.
    BLEND_FACTOR_INV_DEST_COLOR,

    /// The blend factor is (f,f,f,1), where f = min(As, 1-Ad),
    /// As is alpha data from a pixel shader, and Ad is alpha data from a render target.\n
    /// Direct3D counterpart: D3D11_BLEND_SRC_ALPHA_SAT/D3D12_BLEND_SRC_ALPHA_SAT. OpenGL counterpart: GL_SRC_ALPHA_SATURATE.
    BLEND_FACTOR_SRC_ALPHA_SAT,

    /// The blend factor is the constant blend factor set with IDeviceContext::SetBlendFactors().\n
    /// Direct3D counterpart: D3D11_BLEND_BLEND_FACTOR/D3D12_BLEND_BLEND_FACTOR. OpenGL counterpart: GL_CONSTANT_COLOR.
    BLEND_FACTOR_BLEND_FACTOR,

    /// The blend factor is one minus constant blend factor set with IDeviceContext::SetBlendFactors().\n
    /// Direct3D counterpart: D3D11_BLEND_INV_BLEND_FACTOR/D3D12_BLEND_INV_BLEND_FACTOR. OpenGL counterpart: GL_ONE_MINUS_CONSTANT_COLOR.
    BLEND_FACTOR_INV_BLEND_FACTOR,

    /// The blend factor is the second RGB data output from a pixel shader.\n
    /// Direct3D counterpart: D3D11_BLEND_SRC1_COLOR/D3D12_BLEND_SRC1_COLOR. OpenGL counterpart: GL_SRC1_COLOR.
    BLEND_FACTOR_SRC1_COLOR,

    /// The blend factor is 1-RGB, where RGB is the second RGB data output from a pixel shader.\n
    /// Direct3D counterpart: D3D11_BLEND_INV_SRC1_COLOR/D3D12_BLEND_INV_SRC1_COLOR. OpenGL counterpart: GL_ONE_MINUS_SRC1_COLOR.
    BLEND_FACTOR_INV_SRC1_COLOR,

    /// The blend factor is the second alpha (A) data output from a pixel shader.\n
    /// Direct3D counterpart: D3D11_BLEND_SRC1_ALPHA/D3D12_BLEND_SRC1_ALPHA. OpenGL counterpart: GL_SRC1_ALPHA.
    BLEND_FACTOR_SRC1_ALPHA,

    /// The blend factor is 1-A, where A is the second alpha data output from a pixel shader.\n
    /// Direct3D counterpart: D3D11_BLEND_INV_SRC1_ALPHA/D3D12_BLEND_INV_SRC1_ALPHA. OpenGL counterpart: GL_ONE_MINUS_SRC1_ALPHA.
    BLEND_FACTOR_INV_SRC1_ALPHA,

    /// Helper value that stores the total number of blend factors in the enumeration.
    BLEND_FACTOR_NUM_FACTORS
};

/// [D3D11_BLEND_OP]: https://msdn.microsoft.com/en-us/library/windows/desktop/ff476088(v=vs.85).aspx
/// [D3D12_BLEND_OP]: https://msdn.microsoft.com/en-us/library/windows/desktop/dn770340(v=vs.85).aspx
/// [glBlendEquationSeparate]: https://www.opengl.org/wiki/GLAPI/glBlendEquationSeparate
/// This enumeration describes blending operation for RGB or Alpha channels and generally mirrors
/// [D3D11_BLEND_OP][] and [D3D12_BLEND_OP][] enums. It is used by RenderTargetBlendDesc structure to define RGB and Alpha
/// blending operations
/// \sa [D3D11_BLEND_OP on MSDN][D3D11_BLEND_OP], [D3D12_BLEND_OP on MSDN][D3D12_BLEND_OP], [glBlendEquationSeparate on OpenGL.org][glBlendEquationSeparate]
enum BLEND_OP : uint8_t
{
    /// Undefined blend operation
    BLEND_OP_UNDEFINED = 0,

    /// Add source and destination color components.\n
    /// Direct3D counterpart: D3D11_BLEND_OP_ADD/D3D12_BLEND_OP_ADD. OpenGL counterpart: GL_FUNC_ADD.
    BLEND_OP_ADD,

    /// Subtract destination color components from source color components.\n
    /// Direct3D counterpart: D3D11_BLEND_OP_SUBTRACT/D3D12_BLEND_OP_SUBTRACT. OpenGL counterpart: GL_FUNC_SUBTRACT.
    BLEND_OP_SUBTRACT,

    /// Subtract source color components from destination color components.\n
    /// Direct3D counterpart: D3D11_BLEND_OP_REV_SUBTRACT/D3D12_BLEND_OP_REV_SUBTRACT. OpenGL counterpart: GL_FUNC_REVERSE_SUBTRACT.
    BLEND_OP_REV_SUBTRACT,

    /// Compute the minimum of source and destination color components.\n
    /// Direct3D counterpart: D3D11_BLEND_OP_MIN/D3D12_BLEND_OP_MIN. OpenGL counterpart: GL_MIN.
    BLEND_OP_MIN,

    /// Compute the maximum of source and destination color components.\n
    /// Direct3D counterpart: D3D11_BLEND_OP_MAX/D3D12_BLEND_OP_MAX. OpenGL counterpart: GL_MAX.
    BLEND_OP_MAX,

    /// Helper value that stores the total number of blend operations in the enumeration.
    BLEND_OP_NUM_OPERATIONS
};

enum COLOR_MASK : uint8_t
{
    /// Do not store any components.
    COLOR_MASK_NONE  = 0u,

    /// Allow data to be stored in the red component.
    COLOR_MASK_RED   = 1u << 0u,

    /// Allow data to be stored in the green component.
    COLOR_MASK_GREEN = 1u << 1u,

    /// Allow data to be stored in the blue component.
    COLOR_MASK_BLUE  = 1u << 2u,

    /// Allow data to be stored in the alpha component.
    COLOR_MASK_ALPHA = 1u << 3u,

    /// Allow data to be stored in all RGB components.
    COLOR_MASK_RGB   = COLOR_MASK_RED | COLOR_MASK_GREEN | COLOR_MASK_BLUE,

    /// Allow data to be stored in all components.
    COLOR_MASK_ALL   = (COLOR_MASK_RGB | COLOR_MASK_ALPHA)
};
DEFINE_FLAG_ENUM_OPERATORS(COLOR_MASK);

/// Logic operation

/// [D3D12_LOGIC_OP]: https://msdn.microsoft.com/en-us/library/windows/desktop/dn770379(v=vs.85).aspx
/// This enumeration describes logic operation and generally mirrors [D3D12_LOGIC_OP][] enum.
/// It is used by RenderTargetBlendDesc structure to define logic operation.
/// Only available on D3D12 engine
/// \sa [D3D12_LOGIC_OP on MSDN][D3D12_LOGIC_OP]
enum LOGIC_OP : uint8_t
{
    /// Clear the render target.\n
    /// Direct3D12 counterpart: D3D12_LOGIC_OP_CLEAR.
    LOGIC_OP_CLEAR = 0,

    /// Set the render target.\n
    /// Direct3D12 counterpart: D3D12_LOGIC_OP_SET.
    LOGIC_OP_SET,

    /// Copy the render target.\n
    /// Direct3D12 counterpart: D3D12_LOGIC_OP_COPY.
    LOGIC_OP_COPY,

    /// Perform an inverted-copy of the render target.\n
    /// Direct3D12 counterpart: D3D12_LOGIC_OP_COPY_INVERTED.
    LOGIC_OP_COPY_INVERTED,

    /// No operation is performed on the render target.\n
    /// Direct3D12 counterpart: D3D12_LOGIC_OP_NOOP.
    LOGIC_OP_NOOP,

    /// Invert the render target.\n
    /// Direct3D12 counterpart: D3D12_LOGIC_OP_INVERT.
    LOGIC_OP_INVERT,

    /// Perform a logical AND operation on the render target.\n
    /// Direct3D12 counterpart: D3D12_LOGIC_OP_AND.
    LOGIC_OP_AND,

    /// Perform a logical NAND operation on the render target.\n
    /// Direct3D12 counterpart: D3D12_LOGIC_OP_NAND.
    LOGIC_OP_NAND,

    /// Perform a logical OR operation on the render target.\n
    /// Direct3D12 counterpart: D3D12_LOGIC_OP_OR.
    LOGIC_OP_OR,

    /// Perform a logical NOR operation on the render target.\n
    /// Direct3D12 counterpart: D3D12_LOGIC_OP_NOR.
    LOGIC_OP_NOR,

    /// Perform a logical XOR operation on the render target.\n
    /// Direct3D12 counterpart: D3D12_LOGIC_OP_XOR.
    LOGIC_OP_XOR,

    /// Perform a logical equal operation on the render target.\n
    /// Direct3D12 counterpart: D3D12_LOGIC_OP_EQUIV.
    LOGIC_OP_EQUIV,

    /// Perform a logical AND and reverse operation on the render target.\n
    /// Direct3D12 counterpart: D3D12_LOGIC_OP_AND_REVERSE.
    LOGIC_OP_AND_REVERSE,

    /// Perform a logical AND and invert operation on the render target.\n
    /// Direct3D12 counterpart: D3D12_LOGIC_OP_AND_INVERTED.
    LOGIC_OP_AND_INVERTED,

    /// Perform a logical OR and reverse operation on the render target.\n
    /// Direct3D12 counterpart: D3D12_LOGIC_OP_OR_REVERSE.
    LOGIC_OP_OR_REVERSE,

    /// Perform a logical OR and invert operation on the render target.\n
    /// Direct3D12 counterpart: D3D12_LOGIC_OP_OR_INVERTED.
    LOGIC_OP_OR_INVERTED,

    /// Helper value that stores the total number of logical operations in the enumeration.
    LOGIC_OP_NUM_OPERATIONS
};

struct RenderTargetBlendDesc
{
    bool BlendEnable   = false;
    bool LogicOpEnable = false;

    BLEND_FACTOR SrcBlend = BLEND_FACTOR_ONE ;
    BLEND_FACTOR DstBlend = BLEND_FACTOR_ZERO;

    BLEND_OP BlendOp      = BLEND_OP_ADD;

    BLEND_FACTOR SrcBlendAlpha = BLEND_FACTOR_ONE ;
    BLEND_FACTOR DstBlendAlpha = BLEND_FACTOR_ZERO;

    BLEND_OP BlendOpAlpha = BLEND_OP_ADD;

    LOGIC_OP LogicOp      = LOGIC_OP_NOOP;

    COLOR_MASK WriteMask  = COLOR_MASK_ALL;

    constexpr RenderTargetBlendDesc() noexcept {}

    explicit constexpr
    RenderTargetBlendDesc(bool            _blendEnable,
                          bool            _logicOperationEnable  = RenderTargetBlendDesc{}.LogicOpEnable,
                          BLEND_FACTOR    _srcBlend              = RenderTargetBlendDesc{}.SrcBlend,
                          BLEND_FACTOR    _destBlend             = RenderTargetBlendDesc{}.DstBlend,
                          BLEND_OP        _blendOp               = RenderTargetBlendDesc{}.BlendOp,
                          BLEND_FACTOR    _srcBlendAlpha         = RenderTargetBlendDesc{}.SrcBlendAlpha,
                          BLEND_FACTOR    _destBlendAlpha        = RenderTargetBlendDesc{}.DstBlendAlpha,
                          BLEND_OP        _blendOpAlpha          = RenderTargetBlendDesc{}.BlendOpAlpha,
                          LOGIC_OP        _logicOp               = RenderTargetBlendDesc{}.LogicOp,
                          COLOR_MASK      _renderTargetWriteMask = RenderTargetBlendDesc{}.WriteMask)
        : BlendEnable   {_blendEnable          }
        , LogicOpEnable {_logicOperationEnable }
        , SrcBlend      {_srcBlend             }
        , DstBlend      {_destBlend            }
        , BlendOp       {_blendOp              }
        , SrcBlendAlpha {_srcBlendAlpha        }
        , DstBlendAlpha {_destBlendAlpha       }
        , BlendOpAlpha  {_blendOpAlpha         }
        , LogicOp       {_logicOp              }
        , WriteMask     {_renderTargetWriteMask}
    {}

    /// Comparison operator tests if two structures are equivalent

    /// \param [in] rhs - reference to the structure to perform comparison with
    /// \return
    /// - True if all members of the two structures are equal.
    /// - False otherwise
    constexpr bool operator == (const RenderTargetBlendDesc& rhs)const
    {
        return BlendEnable   == rhs.BlendEnable   &&
               LogicOpEnable == rhs.LogicOpEnable &&
               SrcBlend      == rhs.SrcBlend      &&
               DstBlend      == rhs.DstBlend      &&
               BlendOp       == rhs.BlendOp       &&
               SrcBlendAlpha == rhs.SrcBlendAlpha &&
               DstBlendAlpha == rhs.DstBlendAlpha &&
               BlendOpAlpha  == rhs.BlendOpAlpha  &&
               LogicOp       == rhs.LogicOp       &&
               WriteMask     == rhs.WriteMask;
    }
};

struct BlendStateDesc
{
    bool AlphaToCoverageEnable  = false;
    bool IndependentBlendEnable = false;

    RenderTargetBlendDesc RenderTargets[MAX_RENDER_TARGETS];

    constexpr BlendStateDesc() noexcept {}

    constexpr BlendStateDesc(bool                         _alphaToCoverageEnable,
                             bool                         _independentBlendEnable,
                             const RenderTargetBlendDesc& _RT0 = RenderTargetBlendDesc{}) noexcept
        : AlphaToCoverageEnable   {_alphaToCoverageEnable }
        , IndependentBlendEnable  {_independentBlendEnable}
        , RenderTargets           {_RT0}
    {
    }

    constexpr bool operator==(const BlendStateDesc& _rhs) const
    {
        bool bRTsEqual = true;
        for (size_t i = 0; i < MAX_RENDER_TARGETS; ++i)
        {
            if (RenderTargets[i] != _rhs.RenderTargets[i])
            {
                bRTsEqual = false;
                break;
            }
            if (!IndependentBlendEnable) break;
        }

        return bRTsEqual                                         &&
            AlphaToCoverageEnable  == _rhs.AlphaToCoverageEnable &&
            IndependentBlendEnable == _rhs.IndependentBlendEnable;

        static_assert(sizeof(*this) == 82, "Did you add new members to BlendStateDesc? Please handle them here.");
    }

    constexpr bool operator!=(const BlendStateDesc& _rhs) const
    {
        return !(*this == _rhs);
    }
};

}