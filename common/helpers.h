//
// Created by WhoLeb on 28-Oct-25.
// Copyright (c) 2025 Blainnflare. All rights reserved.
//

#pragma once

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
    if (abs(vector.x) > abs(vector.y))
    {
        float len = sqrt(vector.x * vector.x + vector.z * vector.z);
        return Blainn::Vec3(vector.z, 0.0f, -vector.x) / len;
    }
    else
    {
        float len = sqrt(vector.y * vector.y + vector.z * vector.z);
        return Blainn::Vec3(0.0f, vector.z, -vector.y) / len;
    }
}
