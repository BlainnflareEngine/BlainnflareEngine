#pragma once
#include <Jolt/Geometry/AABox.h>

namespace Blainn
{
struct NavmeshVolumeComponent
{
    JPH::AABox LocalBounds = JPH::AABox::sFromTwoPoints({-5, -5, -5}, {5, 5, 5});
    bool IsEnabled = true;

    float CellSize = 0.3f;
    float AgentHeight = 2.0f;
    float AgentRadius = 0.6f;
    float AgentMaxClimb = 0.5f;
    float AgentMaxSlope = 45.0f;
};
} // namespace Blainn