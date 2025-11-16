//
// Created by gorev on 25.09.2025.
//

#pragma once

#define NOMINMAX

#pragma region EASTL common includes

#include <EASTL/algorithm.h>
#include <EASTL/allocator.h>
#include <EASTL/array.h>
#include <EASTL/deque.h>
#include <EASTL/functional.h>
#include <EASTL/hash_map.h>
#include <EASTL/hash_set.h>
#include <EASTL/list.h>
#include <EASTL/map.h>
#include <EASTL/numeric.h>
#include <EASTL/priority_queue.h>
#include <EASTL/queue.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/string.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/weak_ptr.h>
#include <cstdint>

#include <uuid_v4.h>
#include <yaml-cpp/yaml.h>

#include "subsystems/Log.h"
#include "tools/Profiler.h"

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


namespace eastl
{
template <> struct hash<UUIDv4::UUID>
{
    size_t operator()(const UUIDv4::UUID &u) const EA_NOEXCEPT
    {
        return u.hash(); // must return size_t
    }
};
} // namespace eastl


inline eastl::string ToEASTLString(const std::string &s)
{
    return eastl::string(s.c_str());
}
