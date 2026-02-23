//
// Created by WhoLeb on 23-Feb-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#pragma once

struct IDataBlob
{
    virtual void Resize(size_t _newSize) = 0;

    virtual size_t GetSize() const = 0;

    virtual void* GetDataPtr(size_t _offset = 0) = 0;

    virtual const void GetConstDataPtr(size_t _offset = 0) const = 0;

    template<typename T>
    T* GetDataPtr(size_t _offset = 0)
    {
        return static_cast<T*>(GetDataPtr(_offset));
    }

    template<typename T>
    const T* GetConstDataPtr(size_t _offset = 0) const
    {
        return static_cast<const T*>(GetConstDataPtr(_offset));
    }
};