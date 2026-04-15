#include "TerrainService.h"
#include "../repository/TerrainRepository.h"
#include "../../core/DirectoryService.h"
#include "../../ApplicationEventContext.h"
#include "../dto/MeshData.h"
#include "../dto/VertexData.h"
#include "../../common/LoggerUtil.h"
#include <cereal/archives/binary.hpp>
#include <filesystem>
#include <fstream>
#include <cmath>

namespace Metal {
    namespace fs = std::filesystem;

    std::vector<Dependency> TerrainService::getDependencies() {
        return {
            {"TerrainRepository", &terrainRepository},
            {"DirectoryService", &directoryService}
        };
    }

    void TerrainService::onInitialize() {
        ApplicationEventContext::subscribe("TerrainRepository", [this](const Event &event) {
            generateLODMeshes();
        });
        generateLODMeshes();
    }

    void TerrainService::generateLODMeshes() {
        if (directoryService->getEngineMetadataPath().empty()) return;

        fs::path terrainDir = fs::path(directoryService->getEngineMetadataPath()) / "terrain";
        if (!fs::exists(terrainDir)) {
            fs::create_directories(terrainDir);
        }

        int maxLOD = terrainRepository->terrainMaxLOD;
        float totalSize = terrainRepository->terrainSize;

        for (int lod = 0; lod <= maxLOD; ++lod) {
            std::string path = getLODMeshPath(lod);
            if (fs::exists(path)) continue;

            LOG_INFO("Generating terrain LOD " + std::to_string(lod));

            // Grid resolution for this LOD. 
            // Higher LOD means lower resolution (more distance between vertices).
            // At LOD 0, let's say we have 64x64 vertices.
            // But usually, LOD in quadtree means the size of the patch.
            // For the mesh generation, let's just create a unit-sized plane with fixed resolution,
            // and the shader/transform will scale it.
            // Wait, the requirement says "generate one mesh for each LOD of the terrain".
            // Let's use 64x64 for LOD 0, 32x32 for LOD 1, etc.
            int res = 64 >> lod;
            if (res < 2) res = 2;

            MeshData meshData{"terrain_lod_" + std::to_string(lod)};
            float step = 1.0f / (float)(res - 1);

            for (int y = 0; y < res; ++y) {
                for (int x = 0; x < res; ++x) {
                    VertexData v{};
                    v.vertex = glm::vec3((float)x * step - 0.5f, 0.0f, (float)y * step - 0.5f);
                    v.normal = glm::vec3(0.0f, 1.0f, 0.0f);
                    v.uv = glm::vec2((float)x * step, (float)y * step);
                    meshData.data.push_back(v);
                }
            }

            for (int y = 0; y < res - 1; ++y) {
                for (int x = 0; x < res - 1; ++x) {
                    unsigned int i0 = y * res + x;
                    unsigned int i1 = y * res + (x + 1);
                    unsigned int i2 = (y + 1) * res + x;
                    unsigned int i3 = (y + 1) * res + (x + 1);

                    meshData.indices.push_back(i0);
                    meshData.indices.push_back(i2);
                    meshData.indices.push_back(i1);

                    meshData.indices.push_back(i1);
                    meshData.indices.push_back(i2);
                    meshData.indices.push_back(i3);
                }
            }

            std::ofstream os(path, std::ios::binary);
            cereal::BinaryOutputArchive archive(os);
            archive(meshData);
        }
    }

    std::string TerrainService::getLODMeshPath(int lod) const {
        fs::path terrainDir = fs::path(directoryService->getEngineMetadataPath()) / "terrain";
        return (terrainDir / ("lod_" + std::to_string(lod) + ".mesh")).string();
    }
} // Metal
