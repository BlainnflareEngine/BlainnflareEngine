//
// Created by WhoLeb on 19-Feb-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#pragma once

#include <cstdint>

namespace Blainn
{
struct IMemoryAllocator
{
    virtual void* Allocate(size_t _size, const char* _dbgDescription, const char* _dbgFileName, const int32_t _dbgLineNumber) = 0;
    virtual void Free(void* _ptr) = 0;

    virtual void* AllocateAligned(size_t _size, size_t _alignment, const char* _dbgDescription, const char* _dbgFileName, const int32_t _dbgLineNumber) = 0;
    virtual void FreeAligned(void* _ptr) = 0;
};

}