//
// Created by WhoLeb on 02-Mar-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#pragma once
#include "Render/GraphicsTypes.h"
#include "Render/memory/IDataBlob.h"

namespace Blainn
{

struct IEngineFactory
{
    /// Creates a data blob.

    /// \param [in]  _initialSize - The size of the internal data buffer.
    /// \param [in]  _pData       - Pointer to the data to write to the internal buffer.
    ///                             If null, no data will be written.
    /// \param [out] _ppDataBlob  - Memory address where the pointer to the data blob will be written.
    virtual void CreateDataBlob(size_t      _initialSize,
                                const void* _pData,
                                IDataBlob** _ppDataBlob) const = 0;


    /// Enumerates adapters available on this machine.

    /// \param [in]     _minVersion  - Minimum required API version (feature level for Direct3D).
    /// \param [in,out] _numAdapters - The number of adapters. If Adapters is null, this value
    ///                               will be overwritten with the number of adapters available
    ///                               on this system. If Adapters is not null, this value should
    ///                               contain the maximum number of elements reserved in the array
    ///                               pointed to by Adapters. In the latter case, this value
    ///                               is overwritten with the actual number of elements written to
    ///                               Adapters.
    /// \param [out]    _pAdapters - Pointer to the array containing adapter information. If
    ///                            null is provided, the number of available adapters is
    ///                            written to NumAdapters.
    ///
    /// \note OpenGL backend only supports one device; features and properties will have limited information.
    virtual void EnumerateAdapters(Version              _minVersion,
                                   uint32_t&            _numAdapters,
                                   GraphicsAdapterInfo* _pAdapters) const = 0;
};

}
