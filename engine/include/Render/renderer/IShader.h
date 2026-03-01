//
// Created by WhoLeb on 23-Feb-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#pragma once

#include "helpers.h"
#include "IDeviceObject.h"
#include "Render/GraphicsTypes.h"

namespace Blainn
{

using ShaderVersion = Version;

enum SHADER_SOURCE_LANGUAGE : uint8_t
{
    /// Default language (GLSL for OpenGL/OpenGLES/Vulkan devices, HLSL for Direct3D11/Direct3D12 devices)
    SHADER_SOURCE_LANGUAGE_DEFAULT = 0,

    /// The source language is HLSL
    SHADER_SOURCE_LANGUAGE_HLSL,

    /// The source language is GLSL
    SHADER_SOURCE_LANGUAGE_GLSL,

    /// The source language is GLSL that should be compiled verbatim

    /// By default the engine prepends GLSL shader source code with platform-specific
    /// definitions. For instance it adds appropriate #version directive (e.g. `#version 430 core` or
    /// `#version 310 es`) so that the same source will work on different versions of desktop OpenGL and OpenGLES.
    /// When `SHADER_SOURCE_LANGUAGE_GLSL_VERBATIM` is used, the source code will be compiled as is.
    /// Note that shader macros are ignored when compiling GLSL verbatim in OpenGL backend, and an application
    /// should add the macro definitions to the source code.
    SHADER_SOURCE_LANGUAGE_GLSL_VERBATIM,

    /// The source language is Metal shading language (MSL)
    SHADER_SOURCE_LANGUAGE_MSL,

    /// The source language is Metal shading language (MSL) that should be compiled verbatim

    /// Note that shader macros are ignored when compiling MSL verbatim, and an application
    /// should add the macro definitions to the source code.
    SHADER_SOURCE_LANGUAGE_MSL_VERBATIM,

    /// The source language is Metal bytecode
    SHADER_SOURCE_LANGUAGE_MTLB,

    /// The source language is WebGPU shading language (WGSL)
    SHADER_SOURCE_LANGUAGE_WGSL,

    /// The shader source is provided as device-specific bytecode
    /// (e.g. DXBC or DXIL for Direct3D11/Direct3D12, SPIRV for Vulkan, etc.).
    /// The bytecode is used verbatim and no compilation is performed.
    ///
    /// This option is similar to providing the byte code via `ShaderCreateInfo::ByteCode`.
    SHADER_SOURCE_LANGUAGE_BYTECODE,

    SHADER_SOURCE_LANGUAGE_COUNT
};

enum SHADER_COMPILER : uint32_t
{
    /// Default compiler for specific language and API that is selected as follows:
    ///
    ///     - Direct3D11:      legacy HLSL compiler (FXC)
    ///     - Direct3D12:      legacy HLSL compiler (FXC)
    ///     - OpenGL(ES) GLSL: native compiler
    ///     - OpenGL(ES) HLSL: HLSL2GLSL converter and native compiler
    ///     - Vulkan GLSL:     built-in glslang
    ///     - Vulkan HLSL:     built-in glslang (with limited support for Shader Model 6.x)
    ///     - Metal GLSL/HLSL: built-in glslang (HLSL with limited support for Shader Model 6.x)
    ///     - Metal MSL:       native compiler
    SHADER_COMPILER_DEFAULT = 0,

    /// Built-in glslang compiler for GLSL and HLSL.
    SHADER_COMPILER_GLSLANG,

    /// Modern HLSL compiler (DXC) for Direct3D12 and Vulkan with Shader Model 6.x support.
    SHADER_COMPILER_DXC,

    /// Legacy HLSL compiler (FXC) for Direct3D11 and Direct3D12 supporting shader models up to 5.1.
    SHADER_COMPILER_FXC,

    SHADER_COMPILER_LAST = SHADER_COMPILER_FXC,
    SHADER_COMPILER_COUNT
};

enum CREATE_SHADER_SOURCE_INPUT_STREAM_FLAGS : uint32_t
{
    /// No flag.
    CREATE_SHADER_SOURCE_INPUT_STREAM_FLAG_NONE = 0x00,

    /// Do not output any messages if the file is not found or
    /// other errors occur.
    CREATE_SHADER_SOURCE_INPUT_STREAM_FLAG_SILENT = 0x01,
};
DEFINE_FLAG_ENUM_OPERATORS(CREATE_SHADER_SOURCE_INPUT_STREAM_FLAGS);

struct ShaderDesc : public DeviceObjectAttribs
{
    SHADER_TYPE ShaderType = SHADER_TYPE_UNKNOWN;

    /// Whether to use combined texture samplers.

    /// If set to `true`, textures will be combined with texture samplers.
    /// The `CombinedSamplerSuffix` member defines the suffix added to the texture
    /// variable name to get corresponding sampler name. When using combined samplers,
    /// the sampler assigned to the shader resource view is automatically set when
    /// the view is bound. Otherwise, samplers need to be explicitly set similar to other
    /// shader variables.
    ///
    /// This member has no effect if the shader is used in the PSO that uses pipeline resource signature(s).
    bool UseCombinedTextureSamplers = false;

    /// Combined sampler suffix.

    /// If `UseCombinedTextureSamplers` is `true`, defines the suffix added to the
    /// texture variable name to get corresponding sampler name.  For example,
    /// for default value `"_sampler"`, a texture named `"tex"` will be combined
    /// with the sampler named `"tex_sampler"`.
    /// If `UseCombinedTextureSamplers` is `false`, this member is ignored.
    ///
    /// This member has no effect if the shader is used in the PSO that uses pipeline resource signature(s).
    const char* CombinedSamplerSuffix = "_sampler";


    constexpr ShaderDesc() noexcept {}

    constexpr ShaderDesc(const char* _name,
                         SHADER_TYPE _shaderType,
                         bool        _useCombinedTextureSamplers = ShaderDesc{}.UseCombinedTextureSamplers,
                         const char* _combinedSamplerSuffix      = ShaderDesc{}.CombinedSamplerSuffix) :
        DeviceObjectAttribs       {_name                      },
        ShaderType                {_shaderType                },
        UseCombinedTextureSamplers{_useCombinedTextureSamplers},
        CombinedSamplerSuffix     {_combinedSamplerSuffix     }
    {}

    /// Tests if two shader descriptions are equal.

    /// \param [in] _rhs - reference to the structure to compare with.
    ///
    /// \return     true if all members of the two structures *except for the Name* are equal,
    ///             and false otherwise.
    ///
    /// \note   The operator ignores the Name field as it is used for debug purposes and
    ///         doesn't affect the shader properties.
    bool operator==(const ShaderDesc& _rhs) const noexcept
    {
        // Ignore Name. This is consistent with the hasher (HashCombiner<HasherType, ShaderDesc>).
        return ShaderType                 == _rhs.ShaderType                 &&
               UseCombinedTextureSamplers == _rhs.UseCombinedTextureSamplers &&
               SafeStrEqual(CombinedSamplerSuffix, _rhs.CombinedSamplerSuffix);
    }

    bool operator!=(const ShaderDesc& _rhs) const noexcept
    {
        return !(*this == _rhs);
    }
};

/// Shader status
enum SHADER_STATUS : uint32_t
{
    /// Initial shader status.
    SHADER_STATUS_UNINITIALIZED = 0,

    /// The shader is being compiled.
    SHADER_STATUS_COMPILING,

    /// The shader has been successfully compiled
    /// and is ready to be used.
    SHADER_STATUS_READY,

    /// The shader compilation has failed.
    SHADER_STATUS_FAILED
};

struct ShaderMacro
{
    const char* Name       = nullptr;
    const char* Definition = nullptr;

    constexpr ShaderMacro() noexcept {}

    constexpr ShaderMacro(const char* _name,
                          const char* _definition) noexcept
        : Name(_name)
        , Definition(_definition)
    {}

    bool operator==(const ShaderMacro& _rhs) const noexcept
    {
        return SafeStrEqual(Name, _rhs.Name) && SafeStrEqual(Definition, _rhs.Definition);
    }

    bool operator!=(const ShaderMacro& _rhs) const noexcept
    {
        return !(*this == _rhs);
    }
};

struct ShaderMacroArray
{
    const ShaderMacro* Elements = nullptr;
    uint32_t           Count    = 0;

    constexpr ShaderMacroArray() noexcept {}

    constexpr ShaderMacroArray(const ShaderMacro* _array,
                               const uint32_t     _count) noexcept
    : Elements(_array)
    , Count(_count)
    {}

    constexpr bool operator==(const ShaderMacroArray& RHS) const noexcept
    {
        if (Count != RHS.Count)
            return false;

        if ((Count != 0 && Elements == nullptr) || (RHS.Count != 0 && RHS.Elements == nullptr))
            return false;
        for (uint32_t i = 0; i < Count; ++i)
        {
            if (Elements[i] != RHS.Elements[i])
                return false;
        }
        return true;
    }

    constexpr bool operator!=(const ShaderMacroArray& RHS) const noexcept
    {
        return !(*this == RHS);
    }

    explicit constexpr operator bool() const noexcept
    {
        return Elements != nullptr && Count > 0;
    }

    const ShaderMacro& operator[](size_t index) const noexcept
    {
        return Elements[index];
    }
};

enum SHADER_COMPILE_FLAGS : uint32_t
{
    SHADER_COMPILE_FLAG_NONE = 0,

    /// Enable unbounded resource arrays (e.g. `Texture2D g_Texture[]`).
    SHADER_COMPILE_FLAG_ENABLE_UNBOUNDED_ARRAYS = 1u << 0u,

    /// Don't load shader reflection.
    SHADER_COMPILE_FLAG_SKIP_REFLECTION         = 1u << 1u,

    /// Compile the shader asynchronously.
    ///
    /// When this flag is set to true and if the devices supports
    /// AsyncShaderCompilation feature, the shader will be compiled
    /// asynchronously in the background. An application should use
    /// the IShader::GetStatus() method to check the shader status.
    /// If the device does not support asynchronous shader compilation,
    /// the flag is ignored and the shader is compiled synchronously.
    SHADER_COMPILE_FLAG_ASYNCHRONOUS            = 1u << 2u,

    /// Pack matrices in row-major order.
    ///
    /// By default, matrices are laid out in GPU memory in column-major order,
    /// which means that the first four values in a 4x4 matrix represent
    /// the first column, the next four values represent the second column,
    /// and so on.
    ///
    /// If this flag is set, matrices are packed in row-major order, i.e.
    /// they are laid out in memory row-by-row.
    SHADER_COMPILE_FLAG_PACK_MATRIX_ROW_MAJOR   = 1u << 3u,

    /// Convert HLSL to GLSL when compiling HLSL shaders to SPIRV.
    ///
    /// HLSL shaders can be compiled to SPIRV directly using either DXC or glslang.
    /// While glslang supports most HLSL 5.1 features, some Vulkan-specific functionality
    /// is missing. Notably, glslang does not support UAV texture format annotations
    /// (see https://github.com/KhronosGroup/glslang/issues/3790), for example:
    ///
    ///     [[vk::image_format("rgba8")]] RWTexture2D<float4> g_rwTexture;
    ///
    /// This flag provides a workaround by converting HLSL to GLSL before compiling it
    /// to SPIRV. The converter supports specially formatted comments to specify UAV
    /// texture formats:
    ///
    ///     RWTexture2D<float4 /*format = rgba8*/> g_rwTexture;
    ///
    /// Another use case for this flag is to leverage GLSL-specific keywords in HLSL
    /// shaders, such as `gl_DrawID` for multi-draw or manually setting `gl_PointSize`.
    ///
    /// This flag only takes effect when compiling HLSL to SPIRV with glslang.
    /// Since DXC does not support GLSL, this flag is ignored when SHADER_COMPILER_DXC is used.
    SHADER_COMPILE_FLAG_HLSL_TO_SPIRV_VIA_GLSL = 1u << 4u,

    SHADER_COMPILE_FLAG_LAST = SHADER_COMPILE_FLAG_HLSL_TO_SPIRV_VIA_GLSL
};
DEFINE_FLAG_ENUM_OPERATORS(SHADER_COMPILE_FLAGS);

struct ShaderCreateInfo
{
    /// Source file path

    /// If source file path is provided, `Source` and `ByteCode` members must be null
    const char* FilePath = nullptr;

    /// Pointer to the shader source input stream factory.

    /// The factory is used to load the shader source file if FilePath is not null.
    /// It is also used to create additional input streams for shader include files
    // IShaderSourceInputStreamFactory* pShaderSourceStreamFactory DEFAULT_INITIALIZER(nullptr);

    /// Shader source

    /// If shader source is provided, FilePath and ByteCode members must be null
    const char* Source = nullptr;

    /// Compiled shader bytecode.

    /// If shader byte code is provided, FilePath and Source members must be null
    ///
    /// This option is supported for D3D11, D3D12, Vulkan and Metal backends.
    /// For D3D11 and D3D12 backends, DXBC should be provided.
    /// Vulkan backend expects SPIRV bytecode.
    /// Metal backend supports .metallib bytecode to create MTLLibrary
    /// or SPIRV to translate it to MSL and compile (may be slow).
    ///
    /// If SHADER_COMPILE_FLAG_SKIP_REFLECTION flag is not used, the bytecode
    /// must contain reflection information. If shaders were compiled
    /// using fxc, make sure that `/Qstrip_reflect` option is **not** specified.
    /// HLSL shaders need to be compiled against 4.0 profile or higher.
    const void* ByteCode = nullptr;


    union
    {
        /// Length of the source code, when `Source` is not null.

        /// When Source is not null and is not a null-terminated string, this member
        /// should be used to specify the length of the source code.
        /// If `SourceLength` is zero, the source code string is assumed to be
        /// null-terminated.
        size_t SourceLength = 0;


        /// Size of the compiled shader byte code, when `ByteCode` is not null.

        /// Byte code size (in bytes) must not be zero if `ByteCode` is not null.
        size_t ByteCodeSize;
    };

    const char* EntryPoint = "main";

    ShaderMacroArray Macros;

    ShaderDesc       Desc;

    SHADER_SOURCE_LANGUAGE SourceLanguage = SHADER_SOURCE_LANGUAGE_DEFAULT;

    SHADER_COMPILER ShaderCompiler = SHADER_COMPILER_DEFAULT;

    /// HLSL shader model to use when compiling the shader.

    /// When default value is given (0, 0), the engine will attempt to use the highest HLSL shader model
    /// supported by the device. If the shader is created from the byte code, this value
    /// has no effect.
    ///
    /// \note When HLSL source is converted to GLSL, corresponding GLSL/GLESSL version will be used.
    ShaderVersion HLSLVersion = {};

    /// GLSL version to use when creating the shader.

    /// When default value is given (0, 0), the engine will attempt to use the highest GLSL version
    /// supported by the device.
    ShaderVersion GLSLVersion = {};

    /// GLES shading language version to use when creating the shader.

    /// When default value is given (0, 0), the engine will attempt to use the highest GLESSL version
    /// supported by the device.
    ShaderVersion GLESSLVersion = {};

    /// Metal shading language version to use when creating the shader.

    /// When default value is given (0, 0), the engine will attempt to use the highest MSL version
    /// supported by the device.
    ShaderVersion MSLVersion = {};

    SHADER_COMPILE_FLAGS CompileFlags = SHADER_COMPILE_FLAG_NONE;

    /// The reflection information can be queried through
    /// IShader::GetConstantBufferDesc() method.

    /// \note Loading constant buffer reflection introduces some overhead,
    ///       and should be disabled when it is not needed.
    bool LoadConstantBufferReflection = false;

    /// An optional list of GLSL extensions to enable when compiling GLSL source code.
    const char* GLSLExtensions = nullptr;

    /// Emulated array index suffix for WebGPU backend.

    /// An optional suffix to append to the name of emulated array variables to get
    /// the indexed array element name.
    ///
    /// Since WebGPU does not support arrays of resources, Diligent Engine
    /// emulates them by appending an index to the resource name.
    /// For instance, if the suffix is set to `"_"`, resources named
    /// `"g_Tex2D_0"`, `"g_Tex2D_1"`, `"g_Tex2D_2"` will be grouped into an array
    /// of 3 textures named `"g_Tex2D"`. All resources must be the same type
    /// to be grouped into an array.
    ///
    /// When suffix is null or empty, no array emulation is performed.
    ///
    /// \remarks    This member is ignored when compiling shaders for backends other than WebGPU.
    const char* WebGPUEmulatedArrayIndexSuffix = nullptr;

    constexpr ShaderCreateInfo() noexcept
    {}

    // constexpr ShaderCreateInfo(const char*                      _filePath,
    //                            IShaderSourceInputStreamFactory* _pSourceFactory,
    //                            SHADER_SOURCE_LANGUAGE           _sourceLanguage = ShaderCreateInfo{}.SourceLanguage,
    //                            const ShaderDesc&                _desc           = ShaderDesc{}) noexcept
    //     : FilePath                  {_filePath      }
    //     , pShaderSourceStreamFactory{_pSourceFactory}
    //     , Desc                      {_desc          }
    //     , SourceLanguage            {_sourceLanguage}
    // {}

    // constexpr ShaderCreateInfo(const char*                      _filePath,
    //                            IShaderSourceInputStreamFactory* _pSourceFactory,
    //                            const char*                      _EntryPoint,
    //                            const ShaderMacroArray&          _macros         = ShaderCreateInfo{}.Macros,
    //                            SHADER_SOURCE_LANGUAGE           _sourceLanguage = ShaderCreateInfo{}.SourceLanguage,
    //                            const ShaderDesc&                _desc           = ShaderDesc{}) noexcept
    //     : FilePath                  {_filePath      }
    //     , pShaderSourceStreamFactory{_pSourceFactory}
    //     , EntryPoint                {_entryPoint    }
    //     , Macros                    {_macros        }
    //     , Desc                      {_desc          }
    //     , SourceLanguage            {_sourceLanguage}
    // {}

    constexpr ShaderCreateInfo(const char*             _source,
                               size_t                  _sourceLength,
                               const char*             _entryPoint     = ShaderCreateInfo{}.EntryPoint,
                               const ShaderMacroArray& _macros         = ShaderCreateInfo{}.Macros,
                               SHADER_SOURCE_LANGUAGE  _sourceLanguage = ShaderCreateInfo{}.SourceLanguage,
                               const ShaderDesc&       _desc           = ShaderDesc{}) noexcept
        : Source        {_source        }
        , SourceLength  {_sourceLength  }
        , EntryPoint    {_entryPoint    }
        , Macros        {_macros        }
        , Desc          {_desc          }
        , SourceLanguage{_sourceLanguage}
    {}

    constexpr ShaderCreateInfo(const char*            _source,
                               size_t                 _sourceLength,
                               const char*            _entryPoint     = ShaderCreateInfo{}.EntryPoint,
                               SHADER_SOURCE_LANGUAGE _sourceLanguage = ShaderCreateInfo{}.SourceLanguage,
                               const ShaderDesc&      _desc           = ShaderDesc{}) noexcept
        : Source        {_source        }
        , SourceLength  {_sourceLength  }
        , EntryPoint    {_entryPoint    }
        , Desc          {_desc          }
        , SourceLanguage{_sourceLanguage}
    {}

    constexpr ShaderCreateInfo(const void* _byteCode,
                               size_t      _byteCodeSize) noexcept
        : ByteCode    {_byteCode    }
        , ByteCodeSize{_byteCodeSize}
    {}

    /// Comparison operator tests if two structures are equivalent.
    ///
    /// \note   Comparison ignores shader name.
    bool operator==(const ShaderCreateInfo& RHS) const noexcept
    {
        const auto& CI1 = *this;
        const auto& CI2 = RHS;

        if (!SafeStrEqual(CI1.FilePath, CI2.FilePath))
            return false;

        //if (CI1.FilePath != nullptr && CI1.pShaderSourceStreamFactory != CI2.pShaderSourceStreamFactory)
        //    return false;

        if (!SafeStrEqual(CI1.Source, CI2.Source))
            return false;

        if (CI1.SourceLength != CI2.SourceLength)
            return false;

        if ((CI1.ByteCode != nullptr) != (CI2.ByteCode != nullptr))
            return false;

        if ((CI1.ByteCode != nullptr) && (CI2.ByteCode != nullptr))
        {
            if (memcmp(CI1.ByteCode, CI2.ByteCode, CI1.ByteCodeSize) != 0)
                return false;
        }

        if (!SafeStrEqual(CI1.EntryPoint, CI2.EntryPoint))
            return false;

        if (CI1.Macros != CI2.Macros)
            return false;

        if (CI1.Desc != CI2.Desc)
            return false;

        if (CI1.SourceLanguage != CI2.SourceLanguage)
            return false;

        if (CI1.ShaderCompiler != CI2.ShaderCompiler)
            return false;

        if ((CI1.HLSLVersion != CI2.HLSLVersion ||
             CI1.GLSLVersion != CI2.GLSLVersion ||
             CI1.GLESSLVersion != CI2.GLESSLVersion ||
             CI1.MSLVersion != CI2.MSLVersion))
            return false;

        if (CI1.CompileFlags != CI2.CompileFlags)
            return false;

        if (CI1.LoadConstantBufferReflection != CI2.LoadConstantBufferReflection)
            return false;

        if (!SafeStrEqual(CI1.GLSLExtensions, CI2.GLSLExtensions))
            return false;

        if (!SafeStrEqual(CI1.WebGPUEmulatedArrayIndexSuffix, CI2.WebGPUEmulatedArrayIndexSuffix))
            return false;

        return true;
    }

    bool operator!=(const ShaderCreateInfo& RHS) const noexcept
    {
        return !(*this == RHS);
    }
};

/// Describes shader resource type
enum SHADER_RESOURCE_TYPE : uint8_t
{
    /// Shader resource type is unknown
    SHADER_RESOURCE_TYPE_UNKNOWN = 0,

    /// Constant (uniform) buffer
    SHADER_RESOURCE_TYPE_CONSTANT_BUFFER,

    /// Shader resource view of a texture (sampled image)
    SHADER_RESOURCE_TYPE_TEXTURE_SRV,

    /// Shader resource view of a buffer (read-only storage image)
    SHADER_RESOURCE_TYPE_BUFFER_SRV,

    /// Unordered access view of a texture (storage image)
    SHADER_RESOURCE_TYPE_TEXTURE_UAV,

    /// Unordered access view of a buffer (storage buffer)
    SHADER_RESOURCE_TYPE_BUFFER_UAV,

    /// Sampler (separate sampler)
    SHADER_RESOURCE_TYPE_SAMPLER,

    /// Input attachment in a render pass
    SHADER_RESOURCE_TYPE_INPUT_ATTACHMENT,

    /// Acceleration structure
    SHADER_RESOURCE_TYPE_ACCEL_STRUCT,

    SHADER_RESOURCE_TYPE_LAST = SHADER_RESOURCE_TYPE_ACCEL_STRUCT
};


/// Shader resource description
struct ShaderResourceDesc
{
    /// Shader resource name
    const char* Name = nullptr;

    /// Shader resource type, see SHADER_RESOURCE_TYPE.
    SHADER_RESOURCE_TYPE Type = SHADER_RESOURCE_TYPE_UNKNOWN;

    /// Array size. For non-array resource this value is 1.
    uint32_t ArraySize = 0;

    constexpr ShaderResourceDesc() noexcept
    {}

    constexpr ShaderResourceDesc(const char*          _name,
                                 SHADER_RESOURCE_TYPE _type,
                                 uint32_t             _arraySize) noexcept
        : Name     {_name     }
        , Type     {_type     }
        , ArraySize{_arraySize}
    {}

    bool operator==(const ShaderResourceDesc& _rhs) const noexcept
    {
        return Type == _rhs.Type && ArraySize == _rhs.ArraySize && SafeStrEqual(Name, _rhs.Name);
    }
};

/// Describes the basic type of a shader code variable.
enum SHADER_CODE_BASIC_TYPE : uint8_t
{
    /// The type is unknown.
    SHADER_CODE_BASIC_TYPE_UNKNOWN,

    /// Void pointer.
    SHADER_CODE_BASIC_TYPE_VOID,

    /// Boolean (bool).
    SHADER_CODE_BASIC_TYPE_BOOL,

    /// Integer (int).
    SHADER_CODE_BASIC_TYPE_INT,

    /// 8-bit integer (int8).
    SHADER_CODE_BASIC_TYPE_INT8,

    /// 16-bit integer (int16).
    SHADER_CODE_BASIC_TYPE_INT16,

    /// 64-bit integer (int64).
    SHADER_CODE_BASIC_TYPE_INT64,

    /// Unsigned integer (uint).
    SHADER_CODE_BASIC_TYPE_UINT,

    /// 8-bit unsigned integer (uint8).
    SHADER_CODE_BASIC_TYPE_UINT8,

    /// 16-bit unsigned integer (uint16).
    SHADER_CODE_BASIC_TYPE_UINT16,

    /// 64-bit unsigned integer (uint64).
    SHADER_CODE_BASIC_TYPE_UINT64,

    /// Floating-point number (float).
    SHADER_CODE_BASIC_TYPE_FLOAT,

    /// 16-bit floating-point number (half).
    SHADER_CODE_BASIC_TYPE_FLOAT16,

    /// Double-precision (64-bit) floating-point number (double).
    SHADER_CODE_BASIC_TYPE_DOUBLE,

    /// 8-bit float (min8float).
    SHADER_CODE_BASIC_TYPE_MIN8FLOAT,

    /// 10-bit float (min10float).
    SHADER_CODE_BASIC_TYPE_MIN10FLOAT,

    /// 16-bit float (min16float).
    SHADER_CODE_BASIC_TYPE_MIN16FLOAT,

    /// 12-bit int (min12int).
    SHADER_CODE_BASIC_TYPE_MIN12INT,

    /// 16-bit int (min16int).
    SHADER_CODE_BASIC_TYPE_MIN16INT,

    /// 16-bit unsigned int (min16uint).
    SHADER_CODE_BASIC_TYPE_MIN16UINT,

    /// String (string).
    SHADER_CODE_BASIC_TYPE_STRING,

    SHADER_CODE_BASIC_TYPE_COUNT,
};

/// Describes the class of a shader code variable.
enum SHADER_CODE_VARIABLE_CLASS : uint8_t
{
    /// The variable class is unknown.
    SHADER_CODE_VARIABLE_CLASS_UNKNOWN,

    /// The variable is a scalar.
    SHADER_CODE_VARIABLE_CLASS_SCALAR,

    /// The variable is a vector.
    SHADER_CODE_VARIABLE_CLASS_VECTOR,

    /// The variable is a row-major matrix.
    SHADER_CODE_VARIABLE_CLASS_MATRIX_ROWS,

    /// The variable is a column-major matrix.
    SHADER_CODE_VARIABLE_CLASS_MATRIX_COLUMNS,

    /// The variable is a structure.
    SHADER_CODE_VARIABLE_CLASS_STRUCT,

    SHADER_CODE_VARIABLE_CLASS_COUNT,
};

struct ShaderCodeVariableDesc
{
    const char* Name = nullptr;

    const char* TypeName = nullptr;

    SHADER_CODE_VARIABLE_CLASS Class     = SHADER_CODE_VARIABLE_CLASS_UNKNOWN;
    SHADER_CODE_BASIC_TYPE     BasicType = SHADER_CODE_BASIC_TYPE_UNKNOWN    ;


    /// For a matrix type, the number of rows.

    /// \note   For shaders compiled from GLSL, NumRows and NumColumns are swapped.
    uint8_t NumRows = 0;

    /// For a matrix type, the number of columns. For a vector, the number of components.

    /// \note   For shaders compiled from GLSL, NumRows and NumColumns are swapped.
    uint8_t NumColumns = 0;

    /// Offset in bytes from the start of the parent structure and this variable
    uint32_t Offset = 0;

    uint32_t ArraySize = 0;

    /// The number of structure members.
    uint32_t NumMembers = 0;

    /// For a structure, an array of NumMembers struct members
    const ShaderCodeVariableDesc* pMembers = nullptr;


    constexpr ShaderCodeVariableDesc(const char*                _name,
                                     const char*                _typeName,
                                     SHADER_CODE_VARIABLE_CLASS _class,
                                     SHADER_CODE_BASIC_TYPE     _basicType,
                                     uint8_t                    _numRows,
                                     uint8_t                    _numColumns,
                                     uint32_t                   _offset,
                                     uint32_t                   _arraySize = 0) noexcept
        // clang-format off
        : Name      {_name      }
        , TypeName  {_typeName  }
        , Class     {_class     }
        , BasicType {_basicType }
        , NumRows   {_numRows   }
        , NumColumns{_numColumns}
        , Offset    {_offset    }
        , ArraySize {_arraySize }
    // clang-format on
    {}

    constexpr ShaderCodeVariableDesc(const char*            _name,
                                     const char*            _typeName,
                                     SHADER_CODE_BASIC_TYPE _basicType,
                                     uint32_t               _offset,
                                     uint32_t               _arraySize = 0) noexcept
        : Name      {_name                            }
        , TypeName  {_typeName                        }
        , Class     {SHADER_CODE_VARIABLE_CLASS_SCALAR}
        , BasicType {_basicType                       }
        , NumRows   {1                                }
        , NumColumns{1                                }
        , Offset    {_offset                          }
        , ArraySize {_arraySize                       }
    {}

    constexpr ShaderCodeVariableDesc(const char*                   _name,
                                     const char*                   _typeName,
                                     uint32_t                      _numMembers,
                                     const ShaderCodeVariableDesc* _pMembers,
                                     uint32_t                      _offset,
                                     uint32_t                      _arraySize = 0) noexcept
        : Name      {_name                            }
        , TypeName  {_typeName                        }
        , Class     {SHADER_CODE_VARIABLE_CLASS_STRUCT}
        , Offset    {_offset                          }
        , ArraySize {_arraySize                       }
        , NumMembers{_numMembers                      }
        , pMembers  {_pMembers                        }
    {}


    /// Comparison operator tests if two structures are equivalent
    bool operator==(const ShaderCodeVariableDesc& _rhs) const noexcept
    {
        // clang-format off
        if (!SafeStrEqual(Name,     _rhs.Name)     ||
            !SafeStrEqual(TypeName, _rhs.TypeName) ||
            Class      != _rhs.Class      ||
            BasicType  != _rhs.BasicType  ||
            NumRows    != _rhs.NumRows    ||
            NumColumns != _rhs.NumColumns ||
            ArraySize  != _rhs.ArraySize  ||
            Offset     != _rhs.Offset     ||
            NumMembers != _rhs.NumMembers)
            return false;
        // clang-format on

        for (uint32_t i = 0; i < NumMembers; ++i)
        {
            if (pMembers[i] != _rhs.pMembers[i])
                return false;
        }

        return true;
    }
    bool operator!=(const ShaderCodeVariableDesc& _rhs) const noexcept
    {
        return !(*this == _rhs);
    }
};


/// Describes a shader constant buffer.
struct ShaderCodeBufferDesc
{
    /// Buffer size in bytes.
    uint32_t Size = 0;

    /// The number of variables in the buffer.
    uint32_t NumVariables = 0;

    /// An array of NumVariables variables, see ShaderCodeVariableDesc.
    const ShaderCodeVariableDesc* pVariables = nullptr;

    constexpr ShaderCodeBufferDesc() noexcept
    {}


    constexpr ShaderCodeBufferDesc(uint32_t                      _size,
                                   uint32_t                      _numVariables,
                                   const ShaderCodeVariableDesc* _pVariables) noexcept
        // clang-format off
        : Size        {_size        }
        , NumVariables{_numVariables}
        , pVariables  {_pVariables  }
    // clang-format on
    {}

    /// Comparison operator tests if two structures are equivalent
    bool operator==(const ShaderCodeBufferDesc& RHS) const noexcept
    {
        // clang-format off
        if (Size         != RHS.Size ||
            NumVariables != RHS.NumVariables)
            return false;
        // clang-format on

        for (uint32_t i = 0; i < NumVariables; ++i)
        {
            if (pVariables[i] != RHS.pVariables[i])
                return false;
        }

        return true;
    }
    bool operator!=(const ShaderCodeBufferDesc& RHS) const noexcept
    {
        return !(*this == RHS);
    }
};

struct IShader : public IDeviceObject
{
    virtual const ShaderDesc& GetDesc() const override = 0;

    virtual uint32_t GetResourceCount() const = 0;

    virtual void GetResourceDesc(uint32_t            _index,
                                 ShaderResourceDesc& _resourceDesc) const = 0;

    /// For a constant buffer resource, returns the buffer description. See ShaderCodeBufferDesc.

    /// \param [in] _index - Resource index, same as used by GetResourceDesc.
    ///
    /// \return     A pointer to ShaderCodeBufferDesc struct describing the constant buffer.
    ///
    /// This method requires that the `LoadConstantBufferReflection` flag was set to `true`
    /// when the shader was created.
    virtual const ShaderCodeBufferDesc* GetConstantBufferDesc(uint32_t _index) const = 0;

    /// Returns the shader bytecode.

    /// \param [out] _ppBytecode - A pointer to the memory location where
    ///                           a pointer to the byte code will be written.
    /// \param [out] _size       - The size of the byte code.
    ///
    /// For OpenGL, this method returns the full GLSL source.
    ///
    /// The pointer returned by the method remains valid while the
    /// shader object is alive. An application must NOT free the memory.
    virtual void GetByteCode(const void** _ppByteCode,
                             uint64_t     _size) const = 0;

    virtual SHADER_STATUS GetStatus(bool WaitForCompletion = false) = 0;
};

} // namespace Blainn