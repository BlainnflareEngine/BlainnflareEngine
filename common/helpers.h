//
// Created by WhoLeb on 28-Oct-25.
// Copyright (c) 2025 Blainnflare. All rights reserved.
//

#pragma once

#include "aliases.h"
#include <cassert>
#include <cstdlib>
#include <cmath>
#include <cstdint>

#define DEFAULT_COPY_DEFAULT_MOVE(x)                                                                                   \
    x(const x &) = default;                                                                                            \
    x &operator=(const x &) = default;                                                                                 \
    x(x &&) = default;                                                                                                 \
    x &operator=(x &&) = default;

#define NO_COPY_DEFAULT_MOVE(x)                                                                                        \
    x(const x &) = delete;                                                                                             \
    x &operator=(const x &) = delete;                                                                                  \
    x(x &&) = default;                                                                                                 \
    x &operator=(x &&) = default;

#define DEFAULT_COPY_NO_MOVE(x)                                                                                        \
    x(const x &) = default;                                                                                            \
    x &operator=(const x &) = default;                                                                                 \
    x(x &&) = delete;                                                                                                  \
    x &operator=(x &&) = delete;

#define NO_COPY_NO_MOVE(x)                                                                                             \
    x(const x &) = delete;                                                                                             \
    x &operator=(const x &) = delete;                                                                                  \
    x(x &&) = delete;                                                                                                  \
    x &operator=(x &&) = delete;


#if defined(DEBUG) || defined(_DEBUG) || defined(BLAINN_ENABLE_ASSERTIONS)
    #define BF_ASSERT(expr, ...)                                                 \
        do                                                                       \
        {                                                                        \
            const bool bf_assert_ok = static_cast<bool>(expr);                   \
            if (!bf_assert_ok)                                                   \
            {                                                                    \
                BF_FATAL(__VA_ARGS__);                                           \
                assert(bf_assert_ok && "BF_ASSERT failed");                      \
                std::abort();                                                    \
            }                                                                    \
        } while (0)

    #define BF_ASSERT_EXPR(expr) BF_ASSERT((expr), "Debug assertion failed: {}\n", #expr)
#else
    #define BF_ASSERT(expr, ...) do {(void)sizeof(expr);} while (0)
    #define BF_ASSERT_EXPR(expr)          do {(void)sizeof(expr);} while (0)
#endif

// Static assertions stay in Release builds, as they don't have a runtime cost
#define BF_STATIC_ASSERT(expr, message) static_assert((expr), message)
#define BF_STATIC_ASSERT_EXPR(expr) static_assert((expr), "Static assertion failed: " #expr)


template <typename T, typename CastToType = uint32_t> T inline SetBit(T flags, T bit)
{
    return static_cast<T>(static_cast<CastToType>(flags) | static_cast<CastToType>(bit));
}

template <typename T, typename CastToType = uint32_t> T ClearBit(T flags, T bit)
{
    return static_cast<T>(static_cast<CastToType>(flags) & ~static_cast<CastToType>(bit));
}

template <typename T, typename CastToType = uint32_t> bool HasBit(T flags, T bit)
{
    return (static_cast<CastToType>(flags) & static_cast<CastToType>(bit)) != 0;
}

template <typename T, typename CastToType = uint32_t> T inline UpdateBit(T flags, T bit, bool bitValue)
{
    return bitValue ? ClearBit<T, CastToType>(flags, bit) : SetBit<T, CastToType>(flags, bit);
}

inline Blainn::Vec3 GetNormalizedPerpendicular(const Blainn::Vec3 &vector)
{
    if (std::abs(vector.x) > std::abs(vector.y))
    {
        float len = std::sqrt(vector.x * vector.x + vector.z * vector.z);
        return Blainn::Vec3(vector.z, 0.0f, -vector.x) / len;
    }
    else
    {
        float len = std::sqrt(vector.y * vector.y + vector.z * vector.z);
        return Blainn::Vec3(0.0f, vector.z, -vector.y) / len;
    }
}

inline int ConvertDXColorToQColor(float color)
{
    return static_cast<int>(std::round(color * 255.0f));
}

inline float ConvertQColorToDXColor(int color)
{
    return static_cast<float>(color) / 255.0f;
}
