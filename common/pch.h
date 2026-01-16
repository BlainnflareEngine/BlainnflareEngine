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


#include <uuid_v4.h>
#include <yaml-cpp/yaml.h>

#include "subsystems/Log.h"
#include "tools/Profiler.h"


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

#include <lua.hpp>
#include <sol/sol.hpp>

#include <eventpp/callbacklist.h>
#include <entt/entt.hpp>

#pragma region Jolt common includes

#include <Jolt/Jolt.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/Mutex.h>
#include <Jolt/Core/QuickSort.h>
#include <Jolt/Core/UnorderedMap.h>
#include <Jolt/Core/JobSystemSingleThreaded.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Body/BodyLock.h>
#include <Jolt/Physics/Body/BodyLockInterface.h>
#include <Jolt/Physics/Body/BodyManager.h>
#include <Jolt/Physics/Body/MotionType.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhase.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayerInterfaceMask.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/CollideShape.h>
#include <Jolt/Physics/Collision/ContactListener.h>
#include <Jolt/Physics/Collision/NarrowPhaseQuery.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/ScaledShape.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/EActivation.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/StateRecorder.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Renderer/DebugRendererSimple.h>

#pragma endregion

// disable if you dont want to register lua types
#define BLAINN_REGISTER_LUA_TYPES

#ifdef BLAINN_REGISTER_LUA_TYPES
// enable to test lua scripts functionality.
// #define BLAINN_TEST_LUA_SCRIPTS
#endif
