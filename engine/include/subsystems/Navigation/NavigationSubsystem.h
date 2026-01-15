#pragma once

#include "helpers.h"
#include <aliases.h>
#include "DetourNavMesh.h"
#include "NavmeshBuilder.h"
#include "scene/Scene.h"


struct dtNavMesh;
struct dtNavMeshQuery;
struct dtQueryFilter;

namespace Blainn
{

class NavigationSubsystem
{
public:
    NO_COPY_NO_MOVE(NavigationSubsystem);

    static void Init();
    static void Destroy();
    static void Update(float deltaTime);

    static bool LoadNavMesh(const Path &relativePath);
    static bool BakeNavMesh(Scene &scene, Entity navVolumeEntity, const Path &outputRelativePath);
    static void ClearNavMesh();

    static bool FindPath(const Vec3 &start, const Vec3 &end, eastl::vector<Vec3> &outPath);

    static bool IsNavMeshLoaded()
    {
        return m_navMesh != nullptr;
    }

    static void DrawDebugMesh();

    static bool ShouldDrawDebug()
    {
        return s_drawDebug;
    }

    static void SetShouldDrawDebug(bool drawDebug)
    {
        s_drawDebug = drawDebug;
    }

private:
    inline static dtNavMesh *m_navMesh = nullptr;
    inline static dtNavMeshQuery *m_navQuery = nullptr;
    inline static dtQueryFilter *m_filter = nullptr;

    static JPH::AABox CalculateAABBFromPositions(const eastl::vector<float> &positions);
    static eastl::vector<NavMeshInputMesh> CollectGeometryForNavmesh(Scene &scene,
                                                                     const JPH::AABox &navVolumeWorldBounds);
    static bool FindPolysAlongPath(const Vec3 &start, const Vec3 &end, dtPolyRef *polys, int &npolys, int maxPolys);

    inline static bool s_drawDebug = true;
};
} // namespace Blainn