// Navigation/NavmeshBuilder.cpp
#include "Navigation/NavmeshBuilder.h"
#include <DetourNavMesh.h>
#include <DetourNavMeshBuilder.h>

namespace Blainn
{
NavMeshBuildResult NavmeshBuilder::BuildNavMesh(const NavMeshBuildRequest &request)
{
    BF_DEBUG("Building navmesh");

    NavMeshBuildResult result;
    RcContext rc_context;
    if (request.meshes == nullptr || request.bounds == nullptr)
    {
        result.errorMsg = "Invalid navmesh build request";
        return result;
    }

    const eastl::vector<NavMeshInputMesh> &meshes = *request.meshes;
    const JPH::AABox &bounds = *request.bounds;
    const NavMeshBuildSettings &settings = request.settings;

    if (meshes.empty())
    {
        result.errorMsg = "No input geometry provided";
        return result;
    }

    rcConfig config = {};
    config.cs = settings.cellSize;
    config.ch = settings.cellHeight;
    config.walkableSlopeAngle = settings.agentMaxSlope;
    config.walkableHeight = (int)ceilf(settings.agentHeight / config.ch);
    config.walkableClimb = (int)floorf(settings.agentMaxClimb / config.ch);
    config.walkableRadius = (int)ceilf(settings.agentRadius / config.cs);
    config.maxEdgeLen = (int)(settings.edgeMaxLen / config.cs);
    config.maxSimplificationError = settings.edgeMaxError;
    config.minRegionArea = (int)(settings.regionMinSize * settings.regionMinSize);
    config.mergeRegionArea = (int)(settings.regionMergeSize * settings.regionMergeSize);
    config.maxVertsPerPoly = settings.vertsPerPoly;
    config.detailSampleDist = config.cs * settings.detailSampleDist;
    config.detailSampleMaxError = config.ch * settings.detailSampleMaxError;
    config.bmin[0] = bounds.mMin.GetX();
    config.bmin[1] = bounds.mMin.GetY();
    config.bmin[2] = bounds.mMin.GetZ();
    config.bmax[0] = bounds.mMax.GetX();
    config.bmax[1] = bounds.mMax.GetY();
    config.bmax[2] = bounds.mMax.GetZ();
    rcCalcGridSize(config.bmin, config.bmax, config.cs, &config.width, &config.height);

    rcHeightfield *heightfield = rcAllocHeightfield();
    if (!rcCreateHeightfield(&rc_context, *heightfield, config.width, config.height, config.bmin, config.bmax,
                             config.cs, config.ch))
    {
        result.errorMsg = "Could not create solid heightfield";
        return result;
    }

    for (const auto &mesh : meshes)
    {
        if (mesh.positions.size() < 3 || mesh.indices.size() < 3) continue;

        const float *vertices = mesh.positions.data();
        const int *indices = mesh.indices.data();
        int verticesCount = (int)mesh.positions.size() / 3;
        int indicesCount = (int)mesh.indices.size() / 3;

        eastl::vector<unsigned char> areas(indicesCount, RC_WALKABLE_AREA);

        rcRasterizeTriangles(&rc_context, vertices, verticesCount, indices, areas.data(), indicesCount, *heightfield,
                             config.walkableClimb);
    }

    rcFilterLowHangingWalkableObstacles(&rc_context, config.walkableClimb, *heightfield);
    rcFilterLedgeSpans(&rc_context, config.walkableHeight, config.walkableClimb, *heightfield);
    rcFilterWalkableLowHeightSpans(&rc_context, config.walkableHeight, *heightfield);

    rcCompactHeightfield *compactHeightField = rcAllocCompactHeightfield();
    if (!rcBuildCompactHeightfield(&rc_context, config.walkableHeight, config.walkableClimb, *heightfield,
                                   *compactHeightField))
    {
        rcFreeHeightField(heightfield);
        result.errorMsg = "Could not build compact heightfield";
        return result;
    }
    rcFreeHeightField(heightfield);

    if (!rcErodeWalkableArea(&rc_context, config.walkableRadius, *compactHeightField))
    {
        rcFreeCompactHeightfield(compactHeightField);
        result.errorMsg = "Could not erode walkable area";
        return result;
    }

    if (!rcBuildDistanceField(&rc_context, *compactHeightField))
    {
        rcFreeCompactHeightfield(compactHeightField);
        result.errorMsg = "Could not build distance field";
        return result;
    }

    if (!rcBuildRegions(&rc_context, *compactHeightField, 0, config.minRegionArea, config.mergeRegionArea))
    {
        rcFreeCompactHeightfield(compactHeightField);
        result.errorMsg = "Could not build regions";
        return result;
    }

    rcContourSet *contourSet = rcAllocContourSet();
    if (!rcBuildContours(&rc_context, *compactHeightField, config.maxSimplificationError, config.maxEdgeLen,
                         *contourSet))
    {
        rcFreeCompactHeightfield(compactHeightField);
        rcFreeContourSet(contourSet);
        result.errorMsg = "Could not build contours";
        return result;
    }

    rcPolyMesh *polyMesh = rcAllocPolyMesh();
    if (!rcBuildPolyMesh(&rc_context, *contourSet, config.maxVertsPerPoly, *polyMesh))
    {
        rcFreeCompactHeightfield(compactHeightField);
        rcFreeContourSet(contourSet);
        rcFreePolyMesh(polyMesh);
        result.errorMsg = "Could not build poly mesh";
        return result;
    }

    if (polyMesh->npolys > 0)
    {
        const unsigned short WALKABLE_FLAG = 1;
        for (int i = 0; i < polyMesh->npolys; ++i)
        {
            if (polyMesh->areas[i] != RC_NULL_AREA)
            {
                polyMesh->flags[i] = WALKABLE_FLAG;
            }
            else
            {
                polyMesh->flags[i] = 0;
            }
        }
    }

    rcPolyMeshDetail *detailedMesh = rcAllocPolyMeshDetail();
    if (!rcBuildPolyMeshDetail(&rc_context, *polyMesh, *compactHeightField, config.detailSampleDist,
                               config.detailSampleMaxError, *detailedMesh))
    {
        BF_WARN("Could not build detailed mesh");
    }

    rcFreeCompactHeightfield(compactHeightField);
    rcFreeContourSet(contourSet);

    dtNavMeshCreateParams params = {};
    params.verts = polyMesh->verts;
    params.vertCount = polyMesh->nverts;
    params.polys = polyMesh->polys;
    params.polyAreas = polyMesh->areas;
    params.polyFlags = polyMesh->flags;
    params.polyCount = polyMesh->npolys;
    params.nvp = polyMesh->nvp;
    if (detailedMesh)
    {
        params.detailMeshes = detailedMesh->meshes;
        params.detailVerts = detailedMesh->verts;
        params.detailVertsCount = detailedMesh->nverts;
        params.detailTris = detailedMesh->tris;
        params.detailTriCount = detailedMesh->ntris;
    }
    params.walkableHeight = settings.agentHeight;
    params.walkableRadius = settings.agentRadius;
    params.walkableClimb = settings.agentMaxClimb;
    rcVcopy(params.bmin, polyMesh->bmin);
    rcVcopy(params.bmax, polyMesh->bmax);
    params.cs = config.cs;
    params.ch = config.ch;
    params.buildBvTree = true;

    BF_INFO("Poly mesh stats:");
    BF_INFO("  Vertices: {}", polyMesh->nverts);
    BF_INFO("  Polygons: {}", polyMesh->npolys);
    BF_INFO("  Max vertices per polygon: {}", polyMesh->nvp);

    if (detailedMesh)
    {
        BF_INFO("  Detail mesh: {} vertices, {} indices", detailedMesh->nverts, detailedMesh->ntris);
    }
    else
    {
        BF_INFO("  No detail mesh");
    }

    if (polyMesh->npolys == 0)
    {
        BF_ERROR("NO POLYGONS GENERATED! Check input geometry, bounds, and agent settings.");
    }

    unsigned char *navData = nullptr;
    int navDataSize = 0;
    if (!dtCreateNavMeshData(&params, &navData, &navDataSize))
    {
        rcFreePolyMesh(polyMesh);
        rcFreePolyMeshDetail(detailedMesh);
        result.errorMsg = "Could not create Detour navmesh data";
        return result;
    }

    result.navData = navData;
    result.navDataSize = navDataSize;
    result.success = true;

    rcFreePolyMesh(polyMesh);
    rcFreePolyMeshDetail(detailedMesh);

    return result;
}
} // namespace Blainn
