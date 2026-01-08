#include "Navigation/NavigationSubsystem.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshQuery.h"
#include "DetourCommon.h"
#include "DetourAssert.h"
#include "Engine.h"
#include "components/MeshComponent.h"
#include "components/NavMeshVolumeComponent.h"
#include "scene/TransformComponent.h"
#include "file-system/Model.h"
#include <fstream>
#include <AABBHelpers.h>


namespace Blainn
{
void NavigationSubsystem::Init()
{
    m_navQuery = dtAllocNavMeshQuery();
    m_filter = new dtQueryFilter();
    m_filter->setIncludeFlags(0xFFFF);
    m_filter->setExcludeFlags(0);
}


void NavigationSubsystem::Destroy()
{
    if (m_navMesh)
    {
        dtFreeNavMesh(m_navMesh);
        m_navMesh = nullptr;
    }

    if (m_navQuery)
    {
        dtFreeNavMeshQuery(m_navQuery);
        m_navQuery = nullptr;
    }

    delete m_filter;
    m_filter = nullptr;
}


bool NavigationSubsystem::LoadNavMesh(const Path &relativePath)
{
    Path absolutePath = Engine::GetContentDirectory() / relativePath;

    if (!std::filesystem::exists(absolutePath))
    {
        BF_ERROR("File does not exists - {}", absolutePath.string());
        return false;
    }

    std::ifstream file(absolutePath, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        BF_ERROR("Failed to open navmesh file - {}", absolutePath.string());
        return false;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    if (size <= 0)
    {
        BF_ERROR("NavMesh file is empty - {}", absolutePath.string());
        return false;
    }

    eastl::vector<unsigned char> buffer;
    buffer.resize(static_cast<size_t>(size));

    if (!file.read(reinterpret_cast<char *>(buffer.data()), size))
    {
        BF_ERROR("Failed to read navmesh file - {}", absolutePath.string());
        return false;
    }

    dtNavMesh *navMesh = dtAllocNavMesh();
    if (!navMesh)
    {
        BF_ERROR("Failed to allocate dtNavMesh");
        return false;
    }

    dtStatus status = navMesh->init(buffer.data(), static_cast<int>(size), DT_TILE_FREE_DATA);
    if (dtStatusFailed(status))
    {
        dtFreeNavMesh(navMesh);
        BF_ERROR("Failed to initialize dtNavMesh from file - {}", absolutePath.string());
        return false;
    }

    if (m_navMesh)
    {
        dtFreeNavMesh(m_navMesh);
    }

    m_navMesh = navMesh;
    m_navQuery->init(m_navMesh, 2048);

    BF_INFO("NavMesh loaded successfully: {}", absolutePath.string());

    return true;
}


bool NavigationSubsystem::BakeNavMesh(Scene &scene, Entity navVolumeEntity, const Path &outputRelativePath)
{
    if (!navVolumeEntity.HasComponent<NavmeshVolumeComponent>() || !navVolumeEntity.HasComponent<TransformComponent>())
        return false;

    auto &volume = navVolumeEntity.GetComponent<NavmeshVolumeComponent>();
    auto &transform = navVolumeEntity.GetComponent<TransformComponent>();

    JPH::AABox worldBounds = volume.LocalBounds;
    worldBounds = TransformAABox(volume.LocalBounds, transform.GetTransform());

    auto geometry = CollectGeometryForNavmesh(scene, worldBounds);
    if (geometry.empty())
    {
        BF_WARN("No walkable geometry in volume");
        return false;
    }

    NavMeshBuildSettings settings;
    settings.cellSize = volume.CellSize;
    settings.cellHeight = volume.CellSize * 0.5f;
    settings.agentHeight = volume.AgentHeight;
    settings.agentRadius = volume.AgentRadius;
    settings.agentMaxClimb = volume.AgentMaxClimb;
    settings.agentMaxSlope = volume.AgentMaxSlope;

    auto result = NavmeshBuilder::BuildNavMesh(geometry, worldBounds, settings);
    if (!result.success)
    {
        BF_ERROR("NavMesh build failed: {}", result.errorMsg.c_str());
        return false;
    }

    Path absPath = Engine::GetContentDirectory() / outputRelativePath;
    std::filesystem::create_directories(absPath.parent_path());

    std::ofstream file(absPath, std::ios::binary);
    if (!file.is_open())
    {
        dtFreeNavMesh(result.navMesh);
        BF_ERROR("Failed to open output file: {}", absPath.string().c_str());
        return false;
    }

    file.write(reinterpret_cast<const char *>(result.navData), result.navDataSize);

    dtFreeNavMesh(result.navMesh);
    BF_INFO("NavMesh baked: {}", absPath.string().c_str());
    return true;
}


bool NavigationSubsystem::FindPath(const Vec3 &start, const Vec3 &end, eastl::vector<Vec3> &outPath)
{
    static const int MAX_POLYS = 256;
    dtPolyRef polys[MAX_POLYS];
    int npolys = 0;

    if (!FindPolysAlongPath(start, end, polys, npolys, MAX_POLYS))
    {
        outPath.clear();
        return false;
    }

    float straightPath[MAX_POLYS * 3];
    int nstraight = 0;
    m_navQuery->findStraightPath(&start.x, &end.x, polys, npolys, straightPath, nullptr, nullptr, &nstraight, MAX_POLYS,
                                 DT_STRAIGHTPATH_ALL_CROSSINGS);

    if (nstraight == 0)
    {
        outPath.clear();
        return false;
    }

    outPath.resize(nstraight);
    for (int i = 0; i < nstraight; ++i)
    {
        outPath[i] = Vec3(straightPath[i * 3 + 0], straightPath[i * 3 + 1], straightPath[i * 3 + 2]);
    }

    return true;
}


JPH::AABox NavigationSubsystem::CalculateAABBFromPositions(const eastl::vector<float> &positions)
{
    if (positions.size() < 3) return JPH::AABox();

    float minX = positions[0], minY = positions[1], minZ = positions[2];
    float maxX = minX, maxY = minY, maxZ = minZ;

    for (size_t i = 3; i < positions.size(); i += 3)
    {
        float x = positions[i + 0];
        float y = positions[i + 1];
        float z = positions[i + 2];
        if (x < minX) minX = x;
        if (y < minY) minY = y;
        if (z < minZ) minZ = z;
        if (x > maxX) maxX = x;
        if (y > maxY) maxY = y;
        if (z > maxZ) maxZ = z;
    }

    return JPH::AABox(JPH::Vec3(minX, minY, minZ), JPH::Vec3(maxX, maxY, maxZ));
}


eastl::vector<NavMeshInputMesh> NavigationSubsystem::CollectGeometryForNavmesh(Scene &scene,
                                                                               const JPH::AABox &navVolumeWorldBounds)
{
    eastl::vector<NavMeshInputMesh> result;
    auto entities = scene.GetAllEntitiesWith<MeshComponent, TransformComponent>().each();

    for (const auto &[entity, mesh, transform] : entities)
    {
        auto &meshData = mesh.MeshHandle->GetMesh();
        if (meshData.GetAllVertices().empty() || meshData.GetAllIndices().empty()) continue;

        eastl::vector<float> worldPositions;
        worldPositions.reserve(meshData.GetAllVertices().size() * 3);

        Mat4 worldMat = transform.GetTransform();
        for (const auto &vertex : meshData.GetAllVertices())
        {
            Vec4 worldPos = {vertex.position.x, vertex.position.y, vertex.position.z, 1.0f};
            worldPos = Vec4::Transform(worldPos, worldMat);
            worldPositions.push_back(worldPos.x);
            worldPositions.push_back(worldPos.y);
            worldPositions.push_back(worldPos.z);
        }

        JPH::AABox meshAABB = CalculateAABBFromPositions(worldPositions);
        if (!meshAABB.Overlaps(navVolumeWorldBounds)) continue;

        eastl::vector<int> indicesInt;
        indicesInt.reserve(meshData.GetAllIndices().size());
        for (uint32_t idx : meshData.GetAllIndices())
        {
            indicesInt.push_back(static_cast<int>(idx));
        }

        result.push_back({.positions = eastl::move(worldPositions), .indices = eastl::move(indicesInt)});
    }

    return result;
}


bool NavigationSubsystem::FindPolysAlongPath(const Vec3 &start, const Vec3 &end, dtPolyRef *polys, int &npolys,
                                             int maxPolys)
{
    if (!m_navMesh || !m_navQuery) return false;

    float spos[3] = {start.x, start.y, start.z};
    float epos[3] = {end.x, end.y, end.z};

    dtPolyRef startRef, endRef;
    float extents[3] = {2.0f, 4.0f, 2.0f};

    m_navQuery->findNearestPoly(spos, extents, m_filter, &startRef, nullptr);
    m_navQuery->findNearestPoly(epos, extents, m_filter, &endRef, nullptr);

    if (!startRef || !endRef) return false;

    m_navQuery->findPath(startRef, endRef, spos, epos, m_filter, polys, &npolys, maxPolys);
    return npolys > 0;
}
} // namespace Blainn