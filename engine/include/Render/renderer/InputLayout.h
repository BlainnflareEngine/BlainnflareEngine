//
// Created by WhoLeb on 24-Feb-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#pragma once

#include <cstdint>

#include "Render/GraphicsTypes.h"

namespace Blainn
{

#define BLAINN_MAX_LAYOUT_ELEMENTS 16

#define BLAINN_LAYOUT_ELEMENT_AUTO_OFFSET 0xFFFFFFFFFU

#define BLAINN_LAYOUT_ELEMENT_AUTO_STRIDE 0xFFFFFFFFFU

static constexpr uint32_t MAX_LAYOUT_ELEMENTS        = BLAINN_MAX_LAYOUT_ELEMENTS;
static constexpr uint32_t LAYOUT_ELEMENT_AUTO_OFFSET = BLAINN_LAYOUT_ELEMENT_AUTO_OFFSET;
static constexpr uint32_t LAYOUT_ELEMENT_AUTO_STRIDE = BLAINN_LAYOUT_ELEMENT_AUTO_STRIDE;

enum INPUT_ELEMENT_FREQUENCY : uint8_t
{
    /// Frequency is undefined.
    INPUT_ELEMENT_FREQUENCY_UNDEFINED = 0,

    /// Input data is per-vertex data.
    INPUT_ELEMENT_FREQUENCY_PER_VERTEX,

    /// Input data is per-instance data.
    INPUT_ELEMENT_FREQUENCY_PER_INSTANCE,

    /// Helper value that stores the total number of frequencies in the enumeration.
    INPUT_ELEMENT_FREQUENCY_NUM_FREQUENCIES
};

struct LayoutElement
{
    /// HLSL semantic.

    /// Default value (`"ATTRIB"`) allows HLSL shaders to be converted
    /// to GLSL and used in OpenGL backend as well as compiled to SPIRV and used
    /// in Vulkan backend.
    /// Any value other than default will only work in Direct3D11 and Direct3D12 backends.
    const char* HLSLSemantic = "ATTRIB");

    /// Input index of the element that is specified in the vertex shader.

    /// In Direct3D11 and Direct3D12 backends this is the semantic index.
    uint32_t InputIndex        = 0;

    /// Buffer slot index that this element is read from.
    uint32_t BufferSlot        = 0;

    /// Number of components in the element. Allowed values are 1, 2, 3, and 4.
    uint32_t NumComponents     = 0;

    /// Type of the element components, see Diligent::VALUE_TYPE for details.
    VALUE_TYPE ValueType     = Blainn::VT_FLOAT32;

    /// Indicates if the value should be normalized.

    /// For signed and unsigned integer value types
    /// (`VT_INT8`, `VT_INT16`, `VT_INT32`, `VT_UINT8`, `VT_UINT16`, `VT_UINT32`)
    /// indicates if the value should be normalized to [-1,+1] or
    /// [0, 1] range respectively.
    /// 
    /// For floating point types (`VT_FLOAT16` and `VT_FLOAT32`), this member is ignored.
    bool IsNormalized        = True;

    /// Relative offset, in bytes, to the element bits.

    /// If this value is set to `LAYOUT_ELEMENT_AUTO_OFFSET` (default value), the offset will
    /// be computed automatically by placing the element right after the previous one.
    uint32_t RelativeOffset    = LAYOUT_ELEMENT_AUTO_OFFSET;

    /// Stride, in bytes, between two elements, for this buffer slot.

    /// If this value is set to `LAYOUT_ELEMENT_AUTO_STRIDE`, the stride will be
    /// computed automatically assuming that all elements in the same buffer slot are
    /// packed one after another. If the buffer slot contains multiple layout elements,
    /// they all must specify the same stride or use LAYOUT_ELEMENT_AUTO_STRIDE value.
    uint32_t Stride            = LAYOUT_ELEMENT_AUTO_STRIDE;

    /// Frequency of the input data, see Diligent::INPUT_ELEMENT_FREQUENCY for details.
    INPUT_ELEMENT_FREQUENCY Frequency = INPUT_ELEMENT_FREQUENCY_PER_VERTEX;

    /// The number of instances to draw using the same per-instance data before advancing
    /// in the buffer by one element.
    uint32_t InstanceDataStepRate = 1;


    constexpr LayoutElement() noexcept{}

    /// Initializes the structure
    constexpr LayoutElement(uint32_t                 _inputIndex   ,
                            uint32_t                 _bufferSlot   ,
                            uint32_t                 _numComponents,
                            VALUE_TYPE               _valueType    ,
                            bool                     _isNormalized         = LayoutElement{}.IsNormalized,
                            uint32_t                 _relativeOffset       = LayoutElement{}.RelativeOffset,
                            uint32_t                 _stride               = LayoutElement{}.Stride,
                            INPUT_ELEMENT_FREQUENCY  _frequency            = LayoutElement{}.Frequency,
                            uint32_t                 _instanceDataStepRate = LayoutElement{}.InstanceDataStepRate)noexcept
      : InputIndex          {_inputIndex          }
      , BufferSlot          {_bufferSlot          }
      , NumComponents       {_numComponents       }
      , ValueType           {_valueType           }
      , IsNormalized        {_isNormalized        }
      , RelativeOffset      {_relativeOffset      }
      , Stride              {_stride              }
      , Frequency           {_frequency           }
      , InstanceDataStepRate{_instanceDataStepRate}
    {}

    /// Initializes the structure
    constexpr LayoutElement(const char*               _HLSLSemantic ,
                            uint32_t                  _inputIndex   ,
                            uint32_t                  _bufferSlot   ,
                            uint32_t                  _numComponents,
                            VALUE_TYPE                _valueType    ,
                            bool                      _isNormalized         = LayoutElement{}.IsNormalized,
                            uint32_t                  _relativeOffset       = LayoutElement{}.RelativeOffset,
                            uint32_t                  _stride               = LayoutElement{}.Stride,
                            INPUT_ELEMENT_FREQUENCY   _frequency            = LayoutElement{}.Frequency,
                            uint32_t                  _instanceDataStepRate = LayoutElement{}.InstanceDataStepRate)noexcept
      : HLSLSemantic        {_HLSLSemantic        }
      , InputIndex          {_inputIndex          }
      , BufferSlot          {_bufferSlot          }
      , NumComponents       {_numComponents       }
      , ValueType           {_valueType           }
      , IsNormalized        {_isNormalized        }
      , RelativeOffset      {_relativeOffset      }
      , Stride              {_stride              }
      , Frequency           {_frequency           }
      , InstanceDataStepRate{_instanceDataStepRate}
    {}

    /// Initializes the structure
    constexpr LayoutElement(uint32_t                 _inputIndex   ,
                            uint32_t                 _bufferSlot   ,
                            uint32_t                 _numComponents,
                            VALUE_TYPE               _valueType    ,
                            bool                     _isNormalized ,
                            INPUT_ELEMENT_FREQUENCY  _frequency    ,
                            uint32_t                 _instanceDataStepRate = LayoutElement{}.InstanceDataStepRate)noexcept
      : InputIndex          {_inputIndex                   }
      , BufferSlot          {_bufferSlot                   }
      , NumComponents       {_numComponents                }
      , ValueType           {_valueType                    }
      , IsNormalized        {_isNormalized                 }
      , RelativeOffset      {LayoutElement{}.RelativeOffset}
      , Stride              {LayoutElement{}.Stride        }
      , Frequency           {_frequency                    }
      , InstanceDataStepRate{_instanceDataStepRate         }
    {}

    bool operator == (const LayoutElement& _rhs) const
    {
        return InputIndex           == _rhs.InputIndex            &&
               BufferSlot           == _rhs.BufferSlot            &&
               NumComponents        == _rhs.NumComponents         &&
               ValueType            == _rhs.ValueType             &&
               IsNormalized         == _rhs.IsNormalized          &&
               RelativeOffset       == _rhs.RelativeOffset        &&
               Stride               == _rhs.Stride                &&
               Frequency            == _rhs.Frequency             &&
               InstanceDataStepRate == _rhs.InstanceDataStepRate  &&
               SafeStrEqual(HLSLSemantic, _rhs.HLSLSemantic);
    }

    bool operator != (const LayoutElement& _rhs) const
    {
        return !(*this == _rhs);
    }
};


struct InputLayoutDesc
{
    /// Array of layout elements.
    const LayoutElement* LayoutElements   = nullptr;

    /// The number of layout elements in `LayoutElements` array.
    uint32_t             NumElements      = 0;

    constexpr InputLayoutDesc() noexcept {}

    constexpr InputLayoutDesc(const LayoutElement* _layoutElements,
                              uint32_t             _numElements) noexcept
        : LayoutElements{_layoutElements}
        , NumElements   {_numElements   }
    {}

    bool operator == (const InputLayoutDesc& _rhs) const
    {
        if (NumElements != _rhs.NumElements)
            return false;

        for (uint32_t i = 0; i < NumElements; ++i)
        {
            if (LayoutElements[i] != _rhs.LayoutElements[i])
                return false;
        }

        return true;
    }

    bool operator != (const InputLayoutDesc& _rhs) const
    {
        return !(*this == _rhs);
    }
};

}