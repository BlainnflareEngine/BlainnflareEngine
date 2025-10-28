#pragma once

#include "aliases.h"

namespace Blainn
{
struct RayCastResult
{
    uuid bodyId;
    Vec3 hitPoint;
    Vec3 hitNormal;
    float distance;
};
} // namespace Blainn