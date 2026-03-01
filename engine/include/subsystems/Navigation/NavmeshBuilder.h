#pragma once

#include "aliases.h"
#include "subsystems/Log.h"
#include <Jolt/Jolt.h>
#include <Jolt/Geometry/AABox.h>
#include "helpers.h"
#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <Recast.h>

class dtNavMesh;

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

struct NavMeshBuildRequest
{
    const eastl::vector<NavMeshInputMesh> *meshes = nullptr;
    const JPH::AABox *bounds = nullptr;
    NavMeshBuildSettings settings;
};

class RcContext : public rcContext
{
    void doLog(const rcLogCategory category, const char *msg, const int len) override
    {
        (void)category;
        (void)len;
        (void)msg;
        BF_TRACE("Recast navigation: {}", msg);
    }
};

class NavmeshBuilder
{
public:
    NO_COPY_NO_MOVE(NavmeshBuilder);

    static NavMeshBuildResult BuildNavMesh(const NavMeshBuildRequest &request);
};

} // namespace Blainn
