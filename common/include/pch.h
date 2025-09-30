//
// Created by gorev on 25.09.2025.
//

#pragma once

#pragma region EASTL common includes

#include <cstdint>
#include <EASTL/algorithm.h>
#include <EASTL/allocator.h>
#include <EASTL/array.h>
#include <EASTL/deque.h>
#include <EASTL/functional.h>
#include <EASTL/hash_map.h>
#include <EASTL/hash_set.h>
#include <EASTL/list.h>
#include <EASTL/map.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/weak_ptr.h>
#include <EASTL/numeric.h>
#include <EASTL/priority_queue.h>
#include <EASTL/queue.h>
#include <EASTL/string.h>

#pragma endregion

#pragma region New operators for EASTL

inline void *operator new[](size_t size, const char *pName, int flags, unsigned debugFlags, const char *file, int line)
{
    return new uint8_t[size];
}

inline void *operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char *pName, int flags,
                            unsigned debugFlags, const char *file, int line)
{
    return new uint8_t[size];
}

#pragma endregion

#include <uuid_v4.h>
#include <yaml-cpp/yaml.h>

#include "Log.h"
#include "Profiler.h"
#include "aliases.h"


