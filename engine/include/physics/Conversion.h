#pragma once

#include "aliases.h"

namespace Blainn
{

inline Vec3 ToBlainnVec3(JPH::Vec3 vec)
{
    return Vec3(vec.GetX(), vec.GetY(), vec.GetZ());
}
inline Vec4 ToBlainnVec4(JPH::Vec4 vec)
{
    return Vec4(vec.GetX(), vec.GetY(), vec.GetZ(), vec.GetW());
}
inline Quat ToBlainnQuat(JPH::Quat quat)
{
    return Quat(quat.GetX(), quat.GetY(), quat.GetZ(), quat.GetW());
}

inline JPH::Vec3 ToJoltVec3(Vec3 vec)
{
    return JPH::Vec3(vec.x, vec.y, vec.z);
}
inline JPH::RVec3 ToJoltRVec3(Vec3 vec)
{
    return JPH::RVec3(vec.x, vec.y, vec.z);
}
inline JPH::Vec4 ToJoltVec4(Vec4 vec)
{
    return JPH::Vec4(vec.x, vec.y, vec.z, vec.w);
}
inline JPH::Quat ToJoltQuat(Quat quat)
{
    return JPH::Quat(quat.x, quat.y, quat.z, quat.w);
}
} // namespace Blainn