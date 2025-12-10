//
// Created by WhoLeb on 21-Sep-25.
//

#pragma once

//#define BLAINN_ENABLE_PROFILING 1

#if BLAINN_ENABLE_PROFILING
#include <tracy/Tracy.hpp>
#include <cstdlib>
#include <new>

inline void* operator new(std::size_t size)
{
    if (size == 0)
        size = 1;

    void* p = std::malloc(size);
    if (!p)
        throw std::bad_alloc{};

    TracyAlloc(p, size);
    return p;
}

inline void* operator new[](std::size_t size)
{
    if (size == 0)
        size = 1;

    void* p = std::malloc(size);
    if (!p)
        throw std::bad_alloc{};

    TracyAlloc(p, size);
    return p;
}

inline void operator delete(void* ptr) noexcept
{
    if (!ptr) return;
    TracyFree(ptr);
    std::free(ptr);
}

inline void operator delete[](void* ptr) noexcept
{
    if (!ptr) return;
    TracyFree(ptr);
    std::free(ptr);
}

inline void operator delete(void* ptr, std::size_t) noexcept
{
    if (!ptr) return;
    TracyFree(ptr);
    std::free(ptr);
}

inline void operator delete[](void* ptr, std::size_t) noexcept
{
    if (!ptr) return;
    TracyFree(ptr);
    std::free(ptr);
}

/*
 * This is used to mark that a frame has been finished
 */
#define BLAINN_PROFILE_MARK_FRAME           FrameMark

/*
 * These macros are used to include the profilers
 *
 * BLAINN_PROFILE_FUNC should ONLY be used in the beginning of the function
 *
 * BLAINN_PROFILE_SCOPE is for tracing scopes
 *
 * BLAINN_PROFILE_SCOPE_DYNAMIC to trace scopes and add a name
 * (Cherno uses only this vertion)
 *
 * BLAINN_PROFILE_THREAD assigns a name to a thread
 */
#define BLAINN_PROFILE_FUNC(...)            ZoneScoped//(,##__VA_ARGS__)
#define BLAINN_PROFILE_SCOPE(name)          ZoneScopedN(#name)
#define BLAINN_PROFILE_SCOPE_DYNAMIC(NAME)  ZoneScoped; ZoneName(NAME, strlen(NAME))
#define BLAINN_PROFILE_THREAD(...)          tracy::SetThreadName(__VA_ARGS__);

#else

#define BLAINN_PROFILE_MARK_FRAME
#define BLAINN_PROFILE_FUNC(...)
#define BLAINN_PROFILE_SCOPE(...)
#define BLAINN_PROFILE_SCOPE_DYNAMIC(NAME)
#define BLAINN_PROFILE_THREAD(...)

#endif
