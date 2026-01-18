// Navigation/NavmeshBuilder.cpp
#include "Navigation/NavmeshBuilder.h"
#include <DetourNavMesh.h>
#include <DetourNavMeshBuilder.h>

namespace Blainn
{
NavMeshBuildResult NavmeshBuilder::BuildNavMesh(const eastl::vector<NavMeshInputMesh> &meshes,
                                                const JPH::AABox &bounds, const NavMeshBuildSettings &settings)
{
    BF_DEBUG("Building navmesh");

    NavMeshBuildResult result;
    RcContext ctx;

    if (meshes.empty())
    {
        result.errorMsg = "No input geometry provided";
        return result;
    }

    rcConfig cfg = {};
    cfg.cs = settings.cellSize;
    cfg.ch = settings.cellHeight;
    cfg.walkableSlopeAngle = settings.agentMaxSlope;
    cfg.walkableHeight = (int)ceilf(settings.agentHeight / cfg.ch);
    cfg.walkableClimb = (int)floorf(settings.agentMaxClimb / cfg.ch);
    cfg.walkableRadius = (int)ceilf(settings.agentRadius / cfg.cs);
    cfg.maxEdgeLen = (int)(settings.edgeMaxLen / cfg.cs);
    cfg.maxSimplificationError = settings.edgeMaxError;
    cfg.minRegionArea = (int)(settings.regionMinSize * settings.regionMinSize);
    cfg.mergeRegionArea = (int)(settings.regionMergeSize * settings.regionMergeSize);
    cfg.maxVertsPerPoly = settings.vertsPerPoly;
    cfg.detailSampleDist = cfg.cs * settings.detailSampleDist;
    cfg.detailSampleMaxError = cfg.ch * settings.detailSampleMaxError;
    cfg.bmin[0] = bounds.mMin.GetX();
    cfg.bmin[1] = bounds.mMin.GetY();
    cfg.bmin[2] = bounds.mMin.GetZ();
    cfg.bmax[0] = bounds.mMax.GetX();
    cfg.bmax[1] = bounds.mMax.GetY();
    cfg.bmax[2] = bounds.mMax.GetZ();
    rcCalcGridSize(cfg.bmin, cfg.bmax, cfg.cs, &cfg.width, &cfg.height);

    rcHeightfield *solid = rcAllocHeightfield();
    if (!rcCreateHeightfield(&ctx, *solid, cfg.width, cfg.height, cfg.bmin, cfg.bmax, cfg.cs, cfg.ch))
    {
        result.errorMsg = "Could not create solid heightfield";
        return result;
    }

    for (const auto &mesh : meshes)
    {
        if (mesh.positions.size() < 3 || mesh.indices.size() < 3) continue;

        const float *verts = mesh.positions.data();
        const int *tris = mesh.indices.data();
        int nverts = (int)mesh.positions.size() / 3;
        int ntris = (int)mesh.indices.size() / 3;

        eastl::vector<unsigned char> areas(ntris, RC_WALKABLE_AREA);

        rcRasterizeTriangles(&ctx, verts, nverts, tris, areas.data(), ntris, *solid, cfg.walkableClimb);
    }

    rcFilterLowHangingWalkableObstacles(&ctx, cfg.walkableClimb, *solid);
    rcFilterLedgeSpans(&ctx, cfg.walkableHeight, cfg.walkableClimb, *solid);
    rcFilterWalkableLowHeightSpans(&ctx, cfg.walkableHeight, *solid);

    rcCompactHeightfield *chf = rcAllocCompactHeightfield();
    if (!rcBuildCompactHeightfield(&ctx, cfg.walkableHeight, cfg.walkableClimb, *solid, *chf))
    {
        rcFreeHeightField(solid);
        result.errorMsg = "Could not build compact heightfield";
        return result;
    }
    rcFreeHeightField(solid);

    if (!rcErodeWalkableArea(&ctx, cfg.walkableRadius, *chf))
    {
        rcFreeCompactHeightfield(chf);
        result.errorMsg = "Could not erode walkable area";
        return result;
    }

    if (!rcBuildDistanceField(&ctx, *chf))
    {
        rcFreeCompactHeightfield(chf);
        result.errorMsg = "Could not build distance field";
        return result;
    }

    if (!rcBuildRegions(&ctx, *chf, 0, cfg.minRegionArea, cfg.mergeRegionArea))
    {
        rcFreeCompactHeightfield(chf);
        result.errorMsg = "Could not build regions";
        return result;
    }

    rcContourSet *cset = rcAllocContourSet();
    if (!rcBuildContours(&ctx, *chf, cfg.maxSimplificationError, cfg.maxEdgeLen, *cset))
    {
        rcFreeCompactHeightfield(chf);
        rcFreeContourSet(cset);
        result.errorMsg = "Could not build contours";
        return result;
    }

    rcPolyMesh *pmesh = rcAllocPolyMesh();
    if (!rcBuildPolyMesh(&ctx, *cset, cfg.maxVertsPerPoly, *pmesh))
    {
        rcFreeCompactHeightfield(chf);
        rcFreeContourSet(cset);
        rcFreePolyMesh(pmesh);
        result.errorMsg = "Could not build poly mesh";
        return result;
    }

    rcPolyMeshDetail *dmesh = rcAllocPolyMeshDetail();
    if (!rcBuildPolyMeshDetail(&ctx, *pmesh, *chf, cfg.detailSampleDist, cfg.detailSampleMaxError, *dmesh))
    {
        BF_WARN("Could not build detailed mesh");
    }

    rcFreeCompactHeightfield(chf);
    rcFreeContourSet(cset);

    dtNavMeshCreateParams params = {};
    params.verts = pmesh->verts;
    params.vertCount = pmesh->nverts;
    params.polys = pmesh->polys;
    params.polyAreas = pmesh->areas;
    params.polyFlags = pmesh->flags;
    params.polyCount = pmesh->npolys;
    params.nvp = pmesh->nvp;
    if (dmesh)
    {
        params.detailMeshes = dmesh->meshes;
        params.detailVerts = dmesh->verts;
        params.detailVertsCount = dmesh->nverts;
        params.detailTris = dmesh->tris;
        params.detailTriCount = dmesh->ntris;
    }
    params.walkableHeight = settings.agentHeight;
    params.walkableRadius = settings.agentRadius;
    params.walkableClimb = settings.agentMaxClimb;
    rcVcopy(params.bmin, pmesh->bmin);
    rcVcopy(params.bmax, pmesh->bmax);
    params.cs = cfg.cs;
    params.ch = cfg.ch;
    params.buildBvTree = true;

    BF_INFO("Poly mesh stats:");
    BF_INFO("  Verts: {}", pmesh->nverts);
    BF_INFO("  Polys: {}", pmesh->npolys);
    BF_INFO("  Max verts per poly: {}", pmesh->nvp);

    if (dmesh)
    {
        BF_INFO("  Detail mesh: {} verts, {} tris", dmesh->nverts, dmesh->ntris);
    }
    else
    {
        BF_INFO("  No detail mesh");
    }

    if (pmesh->npolys == 0)
    {
        BF_ERROR("NO POLYGONS GENERATED! Check input geometry, bounds, and agent settings.");
    }

    unsigned char *navData = nullptr;
    int navDataSize = 0;
    if (!dtCreateNavMeshData(&params, &navData, &navDataSize))
    {
        rcFreePolyMesh(pmesh);
        rcFreePolyMeshDetail(dmesh);
        result.errorMsg = "Could not create Detour navmesh data";
        return result;
    }

    result.navData = navData;
    result.navDataSize = navDataSize;
    result.success = true;

    rcFreePolyMesh(pmesh);
    rcFreePolyMeshDetail(dmesh);

    return result;
}
} // namespace Blainn