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
#include <limits>
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
    for (auto &scene : Engine::GetSceneManager().GetActiveScenes())
    {
        for (const auto &[entity, id, transform, controllerComp] :
             scene->GetAllEntitiesWith<IDComponent, TransformComponent, AIControllerComponent>().each())
        {
            AIController &controller = controllerComp.aiController;

            Vec3 moveDir;

            // TODO: this now is always in !local space!, need to convert to world space, otherwise movement will be not
            // correct for child objects
            if (controller.GetDesiredDirection(
                    moveDir, {controllerComp.StoppingDistance, controllerComp.GroundOffset}))
            {
                transform.SetTranslation(transform.GetTranslation()
                                         + moveDir * deltaTime * controllerComp.MovementSpeed);

                if (controllerComp.FaceMovementDirection) controller.RotateControlledPawnLerp(moveDir);
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
    const std::streamoff fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    if (fileSize <= 0)
    {
        BF_ERROR("NavMesh file is empty");
        return false;
    }
    if (fileSize > static_cast<std::streamoff>(std::numeric_limits<std::streamsize>::max()))
    {
        BF_ERROR("NavMesh file is too large to load");
        return false;
    }
    const size_t size = static_cast<size_t>(fileSize);
    const std::streamsize readSize = static_cast<std::streamsize>(fileSize);
    if (size > static_cast<size_t>(std::numeric_limits<int>::max()))
    {
        BF_ERROR("NavMesh file is too large for Detour init");
        return false;
    }

    unsigned char *data = (unsigned char *)dtAlloc(size, DT_ALLOC_PERM);
    if (!data)
    {
        BF_ERROR("Failed to allocate memory for navmesh");
        return false;
    }

    if (!file.read(reinterpret_cast<char *>(data), readSize))
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

    BuildDebugNavMesh();

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

    NavMeshBuildRequest request;
    request.meshes = &geometry;
    request.bounds = &worldBounds;
    request.settings = settings;
    auto result = NavmeshBuilder::BuildNavMesh(request);
    if (!result.success || !result.navData || result.navDataSize <= 0)
    {
        BF_ERROR("NavMesh build failed: {}", result.errorMsg.empty() ? "Unknown error" : result.errorMsg.c_str());
        return false;
    }

    BF_INFO("Navmesh build SUCCESS. Size: {} bytes", result.navDataSize);

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

    BuildDebugNavMesh();

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

    if (!m_debugVertexVector.empty()) m_debugVertexVector.clear();
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
    int straightPathCount = 0;
    m_navQuery->findStraightPath(&start.x, &end.x, polys, npolys, straightPath, nullptr, nullptr, &straightPathCount,
                                 MAX_POLYS, DT_STRAIGHTPATH_ALL_CROSSINGS);

    if (straightPathCount == 0)
    {
        outPath.clear();
        return false;
    }

    outPath.resize(straightPathCount);
    for (int i = 0; i < straightPathCount; ++i)
    {
        outPath[i] = Vec3(straightPath[i * 3 + 0], straightPath[i * 3 + 1], straightPath[i * 3 + 2]);
    }

    return true;
}


std::pair<bool, Vec3> NavigationSubsystem::FindRandomPointOnNavMesh()
{
    if (!m_navMesh || !m_navQuery) return {false, Vec3()};

    if (!m_navMesh || !m_navQuery)
    {
        BF_ERROR("NavMesh or NavQuery is null!");
        return {false, Vec3()};
    }

    dtPolyRef randomRef;
    float randomPt[3];

    dtStatus status =
        m_navQuery->findRandomPoint(m_filter, &NavigationSubsystem::RandomFloatCallback, &randomRef, randomPt);

    if (dtStatusFailed(status) || !randomRef) return {false, Vec3()};

    return {true, Vec3(randomPt[0], randomPt[1], randomPt[2])};
}


std::pair<bool, Vec3> NavigationSubsystem::FindRandomPointOnNavMesh(const Vec3 &origin, const float radius)
{
    if (!m_navMesh || !m_navQuery)
    {
        BF_ERROR("NavMesh or NavQuery is not initialized!");
        return {false, Vec3()};
    }

    float originPos[3] = {origin.x, origin.y, origin.z};
    dtPolyRef startRef;
    float startPos[3];

    float extents[3] = {radius, radius * 2.0f, radius};

    dtStatus status = m_navQuery->findNearestPoly(originPos, extents, m_filter, &startRef, startPos);

    if (dtStatusFailed(status) || !startRef)
    {
        BF_WARN("No starting polygon found near origin ({:.1f}, {:.1f}, {:.1f})", origin.x, origin.y, origin.z);
        return {false, Vec3()};
    }

    dtPolyRef randomRef;
    float randomPt[3];

    status = m_navQuery->findRandomPointAroundCircle(startRef, startPos, radius, m_filter,
                                                     &NavigationSubsystem::RandomFloatCallback, &randomRef, randomPt);

    if (dtStatusFailed(status) || !randomRef)
    {
        BF_WARN("Failed to find random point around circle (radius: {:.1f})", radius);
        return {false, Vec3()};
    }

    return {true, Vec3(randomPt[0], randomPt[1], randomPt[2])};
}


void NavigationSubsystem::DrawDebugMesh()
{
    BLAINN_PROFILE_FUNC();
    if (!m_navMesh || !RenderSubsystem::GetInstance().GetDebugRenderer().IsDebugEnabled()) return;

    RenderSubsystem::GetInstance().GetDebugRenderer().DrawLineList(m_debugVertexVector.begin(),
                                                                   m_debugVertexVector.end());
}

void NavigationSubsystem::BuildDebugNavMesh()
{
    if (!m_navMesh) return;

    if (!m_debugVertexVector.empty()) m_debugVertexVector.clear();

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
                const ptrdiff_t vertexOffset = static_cast<ptrdiff_t>(poly->verts[k]) * 3;
                const float *v = &tile->verts[vertexOffset];
                verts.emplace_back(v[0], v[1], v[2]);
            }

            for (int k = 0; k < poly->vertCount; ++k)
            {
                const Vec3 &a = verts[k];
                const Vec3 &b = verts[(k + 1) % poly->vertCount];
                m_debugVertexVector.emplace_back(VertexPositionColor{a, Color(1.0f, 1.0f, 0.0f, 1.0f)});
                m_debugVertexVector.emplace_back(VertexPositionColor{b, Color(1.0f, 1.0f, 0.0f, 1.0f)});
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
