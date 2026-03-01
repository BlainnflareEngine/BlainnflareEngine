//
// Created by gorev on 25.09.2025.
//

#pragma once

#define NOMINMAX

#pragma region EASTL common includes

#include <EASTL/algorithm.h>
#include <EASTL/allocator.h>
#include <EASTL/array.h>
#include <EASTL/vector.h>
#include <EASTL/deque.h>
#include <EASTL/functional.h>
#include <EASTL/hash_map.h>
#include <EASTL/hash_set.h>
#include <EASTL/list.h>
#include <EASTL/map.h>
#include <EASTL/numeric.h>
#include <EASTL/optional.h>
#include <EASTL/priority_queue.h>
#include <EASTL/queue.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/string.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/unordered_map.h>
#include <EASTL/unordered_set.h>
#include <EASTL/weak_ptr.h>
#include <EASTL/chrono.h>

#pragma endregion

#include <cstdint>

#pragma region render includes

#include <Windows.h>
#include <wrl.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <DirectXColors.h>

#include <d3d12.h>
#include <d3d11_1.h>
#include <DirectXTK12/Src/d3dx12.h>
#include <d3dcompiler.h>
#include <DirectXCollision.h>

#include <intsafe.h>
#include <D3D12MemAlloc.h>

#pragma endregion


#include <yaml-cpp/yaml.h>

#include "subsystems/Log.h"
#include "tools/Profiler.h"
#include "tools/UUID.h"


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

inline eastl::string ToEASTLString(const std::string &s)
{
    return eastl::string(s.c_str());
}

#include <eventpp/callbacklist.h>
#include <entt/entt.hpp>

// disable if you dont want to register lua types
#define BLAINN_REGISTER_LUA_TYPES

#ifdef BLAINN_REGISTER_LUA_TYPES
// enable to test lua scripts functionality.
// #define BLAINN_TEST_LUA_SCRIPTS
#endif

#if defined(_MSC_VER)
    #define BF_FORCEINLINE __forceinline
#else
    #define BF_FORCEINLINE inline __attribute__((always_inline))
#endif
