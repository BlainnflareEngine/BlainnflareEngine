#pragma once

#include <Jolt/Geometry/AABox.h>
#include "helpers.h"
#include <Recast.h>

struct dtNavMesh;

namespace Blainn
{
struct NavMeshBuildSettings
{
    float cellSize = 0.3f;
    float cellHeight = 0.2f;
    float agentHeight = 2.0f;
    float agentRadius = 0.6f;
    float agentMaxClimb = 0.5f;
    float agentMaxSlope = 45.0f;
    float regionMinSize = 8.0f;
    float regionMergeSize = 20.0f;
    float edgeMaxLen = 12.0f;
    float edgeMaxError = 1.3f;
    int vertsPerPoly = 6;
    float detailSampleDist = 6.0f;
    float detailSampleMaxError = 1.0f;
};

struct NavMeshBuildResult
{
    unsigned char *navData = nullptr;
    int navDataSize = 0;
    eastl::string errorMsg;
    bool success = false;
};

struct NavMeshInputMesh
{
    eastl::vector<float> positions; // world space
    eastl::vector<int> indices;
};

class RcContext : public rcContext
{
    void doLog(const rcLogCategory category, const char *msg, const int len) override
    {
        BF_TRACE("Recast navigation: {}", msg);
    }
};

class NavmeshBuilder
{
public:
    NO_COPY_NO_MOVE(NavmeshBuilder);

    static NavMeshBuildResult BuildNavMesh(const eastl::vector<NavMeshInputMesh> &meshes, const JPH::AABox &bounds,
                                           const NavMeshBuildSettings &settings = {});
};

} // namespace Blainn