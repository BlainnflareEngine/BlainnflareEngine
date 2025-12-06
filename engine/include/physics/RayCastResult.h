#pragma once

#include "aliases.h"

namespace Blainn
{
struct RayCastResult
{
    uuid entityId;
    Vec3 hitPoint;
    Vec3 hitNormal;
    float distance;
};
} // namespace Blainn