#pragma once

#include "Components/NavmeshVolumeComponent.h"
#include "scene/TransformComponent.h"
#include <Jolt/Geometry/AABox.h>
#include <Jolt/Math/Mat44.h>

namespace Blainn
{
JPH::AABox TransformAABox(const JPH::AABox &localBox, const Mat4 &worldTransform)
{
    using namespace DirectX::SimpleMath;

    Vec3 corners[8] = {
        Vec3(localBox.mMin.GetX(), localBox.mMin.GetY(), localBox.mMin.GetZ()),
        Vec3(localBox.mMin.GetX(), localBox.mMin.GetY(), localBox.mMax.GetZ()),
        Vec3(localBox.mMin.GetX(), localBox.mMax.GetY(), localBox.mMin.GetZ()),
        Vec3(localBox.mMin.GetX(), localBox.mMax.GetY(), localBox.mMax.GetZ()),
        Vec3(localBox.mMax.GetX(), localBox.mMin.GetY(), localBox.mMin.GetZ()),
        Vec3(localBox.mMax.GetX(), localBox.mMin.GetY(), localBox.mMax.GetZ()),
        Vec3(localBox.mMax.GetX(), localBox.mMax.GetY(), localBox.mMin.GetZ()),
        Vec3(localBox.mMax.GetX(), localBox.mMax.GetY(), localBox.mMax.GetZ()),
    };

    JPH::Vec3 min(FLT_MAX, FLT_MAX, FLT_MAX);
    JPH::Vec3 max(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    for (const auto &corner : corners)
    {
        Vec4 localPos(corner.x, corner.y, corner.z, 1.0f);
        Vec4 worldPos = Vec4::Transform(localPos, worldTransform);

        JPH::Vec3 joltPos(worldPos.x, worldPos.y, worldPos.z);
        min = JPH::Vec3::sMin(min, joltPos);
        max = JPH::Vec3::sMax(max, joltPos);
    }

    return JPH::AABox(min, max);
}
} // namespace Blainn