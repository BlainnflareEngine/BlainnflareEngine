#include "Navigation/NavigationSubsystem.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshQuery.h"
#include "DetourCommon.h"
#include "DetourAssert.h"
#include "Engine.h"
#include "Serializer.h"
#include "Render/DebugRenderer.h"
#include "components/AIControllerComponent.h"
#include "components/MeshComponent.h"
#include "components/NavMeshVolumeComponent.h"
#include "scene/TransformComponent.h"
#include "file-system/Model.h"
#include <fstream>
#include <AABBHelpers.h>


namespace Blainn
{
class AIController;
void NavigationSubsystem::Init()
{
    std::random_device rd;
    m_randomGenerator.seed(rd());

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


void NavigationSubsystem::Update(float deltaTime)
{
    if (auto scene = Engine::GetActiveScene())
    {
        for (const auto &[entity, transform, controllerComp] :
             scene->GetAllEntitiesWith<TransformComponent, AIControllerComponent>().each())
        {
            AIController &controller = controllerComp.aiController;

            Vec3 moveDir;
            if (controller.GetDesiredDirection(moveDir, controllerComp.StoppingDistance))
            {
                transform.SetTranslation(transform.GetTranslation()
                                         + moveDir * deltaTime * controllerComp.MovementSpeed);
            }
        }
    }
}


bool NavigationSubsystem::LoadNavMesh(const Path &relativePath)
{
    Path absolutePath = Engine::GetContentDirectory() / relativePath;
    if (!std::filesystem::exists(absolutePath))
    {
        BF_ERROR("File does not exists - {}", absolutePath.string());
        return false;
    }

    std::ifstream file(absolutePath, std::ios::binary);
    if (!file.is_open())
    {
        BF_ERROR("Failed to open navmesh file - {}", absolutePath.string());
        return false;
    }

    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);

    if (size == 0)
    {
        BF_ERROR("NavMesh file is empty");
        return false;
    }

    unsigned char *data = (unsigned char *)dtAlloc(size, DT_ALLOC_PERM);
    if (!data)
    {
        BF_ERROR("Failed to allocate memory for navmesh");
        return false;
    }

    if (!file.read(reinterpret_cast<char *>(data), size))
    {
        dtFree(data);
        BF_ERROR("Failed to read navmesh file");
        return false;
    }

    dtNavMesh *navMesh = dtAllocNavMesh();
    if (!navMesh)
    {
        dtFree(data);
        BF_ERROR("Failed to allocate dtNavMesh");
        return false;
    }

    dtStatus status = navMesh->init(data, static_cast<int>(size), DT_TILE_FREE_DATA);
    if (dtStatusFailed(status))
    {
        dtFree(data);
        dtFreeNavMesh(navMesh);
        BF_ERROR("Failed to initialize dtNavMesh from file");
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
    if (!result.success || !result.navData || result.navDataSize <= 0)
    {
        BF_ERROR("NavMesh build failed: {}", result.errorMsg.empty() ? "Unknown error" : result.errorMsg.c_str());
        return false;
    }

    BF_INFO("Navmesh build SUCCESS. Size: {} bytes", result.navDataSize);
    if (result.navDataSize >= 4)
    {
        BF_INFO("First 4 bytes: {:02X} {:02X} {:02X} {:02X}", result.navData[0], result.navData[1], result.navData[2],
                result.navData[3]);
    }

    Path absPath = Engine::GetContentDirectory() / outputRelativePath;
    std::filesystem::create_directories(absPath.parent_path());

    std::ofstream file(absPath, std::ios::binary);
    if (!file.is_open())
    {
        dtFree(result.navData);
        BF_ERROR("Failed to open output file: {}", absPath.string().c_str());
        return false;
    }

    file.write(reinterpret_cast<const char *>(result.navData), result.navDataSize);
    file.close();

    dtFree(result.navData);

    Path scenePath = Engine::GetContentDirectory() / scene.GetName().c_str();
    YAML::Node sceneNode = YAML::LoadFile(scenePath.string());
    sceneNode["NavMeshData"]["Path"] = outputRelativePath.string();
    std::ofstream fout(scenePath);
    fout << sceneNode;

    BF_INFO("NavMesh baked: {}", absPath.string().c_str());
    return true;
}


void NavigationSubsystem::ClearNavMesh()
{
    if (m_navMesh)
    {
        dtFreeNavMesh(m_navMesh);
        m_navMesh = nullptr;
    }

    if (m_navQuery)
    {
        m_navQuery->init(nullptr, 0);
    }
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


bool NavigationSubsystem::FindRandomPointOnNavMesh(Vec3 &outPoint)
{
    if (!m_navMesh || !m_navQuery) return false;

    if (!m_navMesh || !m_navQuery)
    {
        BF_ERROR("NavMesh or NavQuery is null!");
        return false;
    }

    const dtNavMesh *nav = m_navMesh;
    int maxTiles = nav->getMaxTiles();
    BF_DEBUG("NavMesh tiles: {}", maxTiles);

    int totalPolys = 0;
    for (int i = 0; i < maxTiles; ++i)
    {
        const dtMeshTile *tile = nav->getTile(i);
        if (tile && tile->header)
        {
            BF_DEBUG("Tile {}: {} polys", i, tile->header->polyCount);
            totalPolys += tile->header->polyCount;
        }
    }

    const dtMeshTile *tile = nav->getTile(0);
    if (tile && tile->header)
    {
        BF_DEBUG("First few polygon flags:");
        for (int i = 0; i < eastl::min(5, (int)tile->header->polyCount); ++i)
        {
            const dtPoly *poly = &tile->polys[i];
            BF_DEBUG("Poly {}: flags = {}", i, poly->flags);
        }
    }

    if (totalPolys == 0)
    {
        BF_ERROR("NavMesh has no polygons!");
        return false;
    }

    dtPolyRef randomRef;
    float randomPt[3];

    dtStatus status =
        m_navQuery->findRandomPoint(m_filter, &NavigationSubsystem::RandomFloatCallback, &randomRef, randomPt);

    if (dtStatusFailed(status) || !randomRef) return false;

    outPoint = Vec3(randomPt[0], randomPt[1], randomPt[2]);
    return true;
}


bool NavigationSubsystem::FindRandomPointOnNavMesh(Vec3 &outPoint, const Vec3 &origin, const float radius)
{
    if (!m_navMesh || !m_navQuery)
    {
        BF_ERROR("NavMesh or NavQuery is not initialized!");
        return false;
    }

    float originPos[3] = {origin.x, origin.y, origin.z};
    dtPolyRef startRef;
    float startPos[3];

    float extents[3] = {radius, radius * 2.0f, radius};

    dtStatus status = m_navQuery->findNearestPoly(originPos, extents, m_filter, &startRef, startPos);

    if (dtStatusFailed(status) || !startRef)
    {
        BF_WARN("No starting polygon found near origin ({:.1f}, {:.1f}, {:.1f})", origin.x, origin.y, origin.z);
        return false;
    }

    dtPolyRef randomRef;
    float randomPt[3];

    status = m_navQuery->findRandomPointAroundCircle(startRef, startPos, radius, m_filter,
                                                     &NavigationSubsystem::RandomFloatCallback, &randomRef, randomPt);

    if (dtStatusFailed(status) || !randomRef)
    {
        BF_WARN("Failed to find random point around circle (radius: {:.1f})", radius);
        return false;
    }

    outPoint = Vec3(randomPt[0], randomPt[1], randomPt[2]);
    return true;
}


void NavigationSubsystem::DrawDebugMesh()
{
    if (!m_navMesh) return;

    const dtNavMesh *nav = m_navMesh;
    const int numTiles = nav->getMaxTiles();

    for (int i = 0; i < numTiles; ++i)
    {
        const dtMeshTile *tile = nav->getTile(i);
        if (!tile || !tile->header) continue;

        for (int j = 0; j < tile->header->polyCount; ++j)
        {
            const dtPoly *poly = &tile->polys[j];
            if (poly->vertCount < 3) continue;

            eastl::vector<Vec3> verts;
            for (int k = 0; k < poly->vertCount; ++k)
            {
                const float *v = &tile->verts[poly->verts[k] * 3];
                verts.emplace_back(v[0], v[1], v[2]);
            }

            for (int k = 0; k < poly->vertCount; ++k)
            {
                const Vec3 &a = verts[k];
                const Vec3 &b = verts[(k + 1) % poly->vertCount];
                RenderSubsystem::GetInstance().GetDebugRenderer().DrawLine(a, b, Color(0.0f, 1.0f, 0.0f, 1.0f));
            }
        }
    }
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


float NavigationSubsystem::RandomFloatCallback()
{
    return m_uniformDist(m_randomGenerator);
}
} // namespace Blainn