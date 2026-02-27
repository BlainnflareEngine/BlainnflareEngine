//
// Created by WhoLeb on 24-Feb-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#pragma once

#include <cstdint>
#include "Render/GraphicsTypes.h"

namespace Blainn
{

/// Stencil operation

/// [D3D11_STENCIL_OP]: https://msdn.microsoft.com/en-us/library/windows/desktop/ff476219(v=vs.85).aspx
/// [D3D12_STENCIL_OP]: https://msdn.microsoft.com/en-us/library/windows/desktop/dn770409(v=vs.85).aspx
/// This enumeration describes the stencil operation and generally mirrors
/// [D3D11_STENCIL_OP][]/[D3D12_STENCIL_OP][] enumeration.
/// It is used by StencilOpDesc structure to describe the stencil fail, depth fail
/// and stencil pass operations
enum STENCIL_OP : uint8_t
{
    /// Undefined operation.
    STENCIL_OP_UNDEFINED = 0,

    /// Keep the existing stencil data.\n
    /// Direct3D counterpart: D3D11_STENCIL_OP_KEEP/D3D12_STENCIL_OP_KEEP. OpenGL counterpart: GL_KEEP.
    STENCIL_OP_KEEP      = 1,

    /// Set the stencil data to 0.\n
    /// Direct3D counterpart: D3D11_STENCIL_OP_ZERO/D3D12_STENCIL_OP_ZERO. OpenGL counterpart: GL_ZERO.
    STENCIL_OP_ZERO      = 2,

    /// Set the stencil data to the reference value set by calling IDeviceContext::SetStencilRef().\n
    /// Direct3D counterpart: D3D11_STENCIL_OP_REPLACE/D3D12_STENCIL_OP_REPLACE. OpenGL counterpart: GL_REPLACE.
    STENCIL_OP_REPLACE   = 3,

    /// Increment the current stencil value, and clamp to the maximum representable unsigned value.\n
    /// Direct3D counterpart: D3D11_STENCIL_OP_INCR_SAT/D3D12_STENCIL_OP_INCR_SAT. OpenGL counterpart: GL_INCR.
    STENCIL_OP_INCR_SAT  = 4,

    /// Decrement the current stencil value, and clamp to 0.\n
    /// Direct3D counterpart: D3D11_STENCIL_OP_DECR_SAT/D3D12_STENCIL_OP_DECR_SAT. OpenGL counterpart: GL_DECR.
    STENCIL_OP_DECR_SAT  = 5,

    /// Bitwise invert the current stencil buffer value.\n
    /// Direct3D counterpart: D3D11_STENCIL_OP_INVERT/D3D12_STENCIL_OP_INVERT. OpenGL counterpart: GL_INVERT.
    STENCIL_OP_INVERT    = 6,

    /// Increment the current stencil value, and wrap the value to zero when incrementing
    /// the maximum representable unsigned value. \n
    /// Direct3D counterpart: D3D11_STENCIL_OP_INCR/D3D12_STENCIL_OP_INCR. OpenGL counterpart: GL_INCR_WRAP.
    STENCIL_OP_INCR_WRAP = 7,

    /// Decrement the current stencil value, and wrap the value to the maximum representable
    /// unsigned value when decrementing a value of zero.\n
    /// Direct3D counterpart: D3D11_STENCIL_OP_DECR/D3D12_STENCIL_OP_DECR. OpenGL counterpart: GL_DECR_WRAP.
    STENCIL_OP_DECR_WRAP = 8,

    /// Helper value that stores the total number of stencil operations in the enumeration.
    STENCIL_OP_NUM_OPS
};


/// Describes stencil operations that are performed based on the results of depth test.

/// [D3D11_DEPTH_STENCILOP_DESC]: https://msdn.microsoft.com/en-us/library/windows/desktop/ff476109(v=vs.85).aspx
/// [D3D12_DEPTH_STENCILOP_DESC]: https://msdn.microsoft.com/en-us/library/windows/desktop/dn770355(v=vs.85).aspx
/// The structure generally mirrors [D3D11_DEPTH_STENCILOP_DESC][]/[D3D12_DEPTH_STENCILOP_DESC][] structure.
/// It is used by DepthStencilStateDesc structure to describe the stencil
/// operations for the front and back facing polygons.
struct StencilOpDesc
{
    /// The stencil operation to perform when stencil testing fails.
    /// Default value: STENCIL_OP_KEEP.
    STENCIL_OP          StencilFailOp       = STENCIL_OP_KEEP;

    /// The stencil operation to perform when stencil testing passes and depth testing fails.
    /// Default value: STENCIL_OP_KEEP.
    STENCIL_OP          StencilDepthFailOp  = STENCIL_OP_KEEP;

    /// The stencil operation to perform when stencil testing and depth testing both pass.
    /// Default value: STENCIL_OP_KEEP.
    STENCIL_OP          StencilPassOp       = STENCIL_OP_KEEP;

    /// A function that compares stencil data against existing stencil data.
    /// Default value: COMPARISON_FUNC_ALWAYS. See COMPARISON_FUNCTION.
    COMPARISON_FUNCTION StencilFunc         = COMPARISON_FUNC_ALWAYS;

    constexpr StencilOpDesc() noexcept {}

    constexpr StencilOpDesc(STENCIL_OP          _stencilFailOp     ,
                            STENCIL_OP          _stencilDepthFailOp,
                            STENCIL_OP          _stencilPassOp     ,
                            COMPARISON_FUNCTION _stencilFunc       ) noexcept
      : StencilFailOp      {_stencilFailOp     }
      , StencilDepthFailOp {_stencilDepthFailOp}
      , StencilPassOp      {_stencilPassOp     }
      , StencilFunc        {_stencilFunc       }
    {}

    /// Tests if two structures are equivalent
    constexpr bool operator== (const StencilOpDesc& rhs) const
    {
        return StencilFailOp      == rhs.StencilFailOp      &&
               StencilDepthFailOp == rhs.StencilDepthFailOp &&
               StencilPassOp      == rhs.StencilPassOp      &&
               StencilFunc        == rhs.StencilFunc;
        static_assert(sizeof(*this) == 4, "Did you add new members to StencilOpDesc? Please handle them here.");
    }
    constexpr bool operator!= (const StencilOpDesc& rhs) const
    {
        return !(*this == rhs);
    }
};


/// Depth stencil state description

/// [D3D11_DEPTH_STENCIL_DESC]: https://msdn.microsoft.com/en-us/library/windows/desktop/ff476110(v=vs.85).aspx
/// [D3D12_DEPTH_STENCIL_DESC]: https://msdn.microsoft.com/en-us/library/windows/desktop/dn770356(v=vs.85).aspx
/// This structure describes the depth stencil state and is part of the GraphicsPipelineDesc.
/// The structure generally mirrors [D3D11_DEPTH_STENCIL_DESC][]/[D3D12_DEPTH_STENCIL_DESC][]
/// structure.
struct DepthStencilStateDesc
{
    /// Enable depth-stencil operations. When it is set to False,
    /// depth test always passes, depth writes are disabled,
    /// and no stencil operations are performed. Default value: true.
    bool                DepthEnable         = true;

    /// Enable or disable writes to a depth buffer. Default value: true.
    bool                DepthWriteEnable    = true;

    /// A function that compares depth data against existing depth data.
    /// See COMPARISON_FUNCTION for details.
    /// Default value: COMPARISON_FUNC_LESS.
    COMPARISON_FUNCTION DepthFunc           = COMPARISON_FUNC_LESS;

    /// Enable stencil operations. Default value: False.
    bool                StencilEnable       = false;

    /// Identify which bits of the depth-stencil buffer are accessed when reading stencil data.
    /// Default value: 0xFF.
    uint8_t             StencilReadMask     = 0xFF;

    /// Identify which bits of the depth-stencil buffer are accessed when writing stencil data.
    /// Default value: 0xFF.
    uint8_t             StencilWriteMask    = 0xFF;

    /// Identify stencil operations for the front-facing triangles, see StencilOpDesc.
    StencilOpDesc FrontFace;

    /// Identify stencil operations for the back-facing triangles, see StencilOpDesc.
    StencilOpDesc BackFace;

    constexpr DepthStencilStateDesc() noexcept {}

    constexpr DepthStencilStateDesc(bool                _depthEnable     ,
                                    bool                _depthWriteEnable,
                                    COMPARISON_FUNCTION _depthFunc        = DepthStencilStateDesc{}.DepthFunc       ,
                                    bool                _stencilEnable    = DepthStencilStateDesc{}.StencilEnable   ,
                                    uint8_t             _stencilReadMask  = DepthStencilStateDesc{}.StencilReadMask ,
                                    uint8_t             _stencilWriteMask = DepthStencilStateDesc{}.StencilWriteMask,
                                    StencilOpDesc       _frontFace        = StencilOpDesc{},
                                    StencilOpDesc       _backFace         = StencilOpDesc{}) noexcept
      : DepthEnable     {_depthEnable     }
      , DepthWriteEnable{_depthWriteEnable}
      , DepthFunc       {_depthFunc       }
      , StencilEnable   {_stencilEnable   }
      , StencilReadMask {_stencilReadMask }
      , StencilWriteMask{_stencilWriteMask}
      , FrontFace       {_frontFace       }
      , BackFace        {_backFace        }
    {}

    /// Tests if two structures are equivalent
    constexpr bool operator== (const DepthStencilStateDesc& _rhs) const
    {
        return  DepthEnable      == _rhs.DepthEnable      &&
                DepthWriteEnable == _rhs.DepthWriteEnable &&
                DepthFunc        == _rhs.DepthFunc        &&
                StencilEnable    == _rhs.StencilEnable    &&
                StencilReadMask  == _rhs.StencilReadMask  &&
                StencilWriteMask == _rhs.StencilWriteMask &&
                FrontFace        == _rhs.FrontFace        &&
                BackFace         == _rhs.BackFace;
        static_assert(sizeof(*this) == 14, "Did you add new members to DepthStencilStateDesc? Please handle them here.");
    }

    constexpr bool operator != (const DepthStencilStateDesc& _rhs) const
    {
        return !(*this == _rhs);
    }
};

} // namespace Blainn