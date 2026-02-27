//
// Created by WhoLeb on 27-Feb-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#pragma once

#include "IDeviceObject.h"

namespace Blainn
{
struct ResourceMappingEntry
{
    const char* Name = nullptr;

    IDeviceObject* pObject = nullptr;

    /// For array resources, index in the array
    uint32_t ArrayIndex = 0;


    constexpr ResourceMappingEntry() noexcept
    {}

    constexpr ResourceMappingEntry(const char*    _name,
                                   IDeviceObject* _pObject,
                                   uint32_t       _arrayIndex = 0) noexcept
        : Name      {_name      }
        , pObject   {_pObject   }
        , ArrayIndex{_arrayIndex}
    {}
};

struct ResourceMappingCreateInfo
{
    const ResourceMappingEntry* pEntries = nullptr;

    uint32_t                    NumEntries;

    constexpr ResourceMappingCreateInfo() noexcept
    {}

    constexpr ResourceMappingCreateInfo(const ResourceMappingEntry* _pEntries,
                                              uint32_t              _numEntries) noexcept
        : pEntries  {_pEntries  }
        , NumEntries{_numEntries}
    {}
};


/// Resource mapping

/// This interface provides mapping between literal names and resource pointers.
/// It is created by IRenderDevice::CreateResourceMapping().
/// \remarks Resource mapping holds strong references to all objects it keeps.
struct IResourceMapping
{
    /// Adds a resource to the mapping.

    /// \param [in] _name - Resource name.
    /// \param [in] _pObject - Pointer to the object.
    /// \param [in] _bIsUnique - Flag indicating if a resource with the same name
    ///                         is allowed to be found in the mapping. In the latter
    ///                         case, the new resource replaces the existing one.
    virtual void AddResource(const char*    _name     ,
                             IDeviceObject* _pObject  ,
                             bool           _bIsUnique) = 0;

    /// Adds resource array to the mapping.

    /// \param [in] _name - Resource array name.
    /// \param [in] _startIndex - First index in the array, where the first element will be inserted
    /// \param [in] _ppObjects - Pointer to the array of objects.
    /// \param [in] _numElements - Number of elements to add
    /// \param [in] _bIsUnique - Flag indicating if a resource with the same name
    ///                         is allowed to be found in the mapping. In the latter
    ///                         case, the new resource replaces the existing one.
    virtual void AddResourceArray(const char*           _name       ,
                                  uint32_t              _startIndex ,
                                  IDeviceObject* const* _ppObjects  ,
                                  uint32_t              _numElements,
                                  bool                  _bIsUnique  ) = 0;


    /// Removes a resource from the mapping using its literal name.

    /// \param [in] _name - Name of the resource to remove.
    /// \param [in] _arrayIndex - For array resources, index in the array
    virtual void RemoveResourceByName(const char* _name,
                                      uint32_t    _arrayIndex = 0) = 0;


    /// Finds a resource in the mapping.

    /// \param [in] _name        - Resource name.
    /// \param [in] _arrayIndex  - for arrays, index of the array element.
    ///
    /// \return Pointer to the object with the given name and array index.
    virtual IDeviceObject* GetResource(const char* _name,
                                       uint32_t    _arrayIndex = 0) = 0;

    /// Returns the size of the resource mapping, i.e. the number of objects.
    virtual size_t GetSize() = 0;
};

}