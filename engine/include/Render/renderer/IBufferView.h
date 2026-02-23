//
// Created by WhoLeb on 23-Feb-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#pragma once
#include "IDeviceObject.h"
#include "Render/GraphicsTypes.h"

namespace Blainn
{
struct BufferFormat
{
    VALUE_TYPE ValueType     = VT_UNDEFINED;

    // Number of components. Allowed values 1, 2, 3, 4
    uint8_t    NumComponents = 0;

    bool       IsNormalized  = false;

    constexpr BufferFormat() noexcept {};

    constexpr BufferFormat(
            VALUE_TYPE  _valueType,
            uint8_t     _numComponents,
            bool        _isNormalized) noexcept
        : ValueType    {_valueType    }
        , NumComponents{_numComponents}
        , IsNormalized {_isNormalized }
    {}

    constexpr bool operator == (const BufferFormat& _rhs)const
    {
        return ValueType     == _rhs.ValueType &&
               NumComponents == _rhs.NumComponents &&
               IsNormalized  == _rhs.IsNormalized;
    }
};

struct BufferViewDesc : public DeviceObjectAttribs
{
    BUFFER_VIEW_TYPE ViewType = BUFFER_VIEW_UNDEFINED;
    BufferFormat     Format;

    uint64_t ByteOffset = 0;
    uint64_t ByteWidth  = 0;

    constexpr BufferViewDesc() noexcept {}

    explicit constexpr
    BufferViewDesc(const char*      _name,
                   BUFFER_VIEW_TYPE _viewType,
                   BufferFormat     _format     = BufferViewDesc{}.Format,
                   uint64_t         _byteOffset = BufferViewDesc{}.ByteOffset,
                   uint64_t         _byteWidth  = BufferViewDesc{}.ByteWidth) noexcept :
        DeviceObjectAttribs {_name      },
        ViewType            {_viewType  },
        Format              {_format    },
        ByteOffset          {_byteOffset},
        ByteWidth           {_byteWidth }
    {}

    constexpr bool operator==(const BufferViewDesc& RHS) const
    {
        // Ignore name.
        return //strcmp(Name, RHS.Name) == 0 &&
               ViewType  == RHS.ViewType   &&
               ByteOffset== RHS.ByteOffset &&
               ByteWidth == RHS.ByteWidth  &&
               Format    == RHS.Format;
    }
};

struct IBufferView : public IDeviceObject
{
    virtual const BufferViewDesc& GetDesc() const override = 0;

    virtual struct IBuffer* GetBuffer() const = 0;
};
} // namespace Blainn
