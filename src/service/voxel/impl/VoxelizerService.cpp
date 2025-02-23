#include "VoxelizerService.h"

#include <stb_image.h>
#include <thread>
#include <cereal/archives/binary.hpp>

#include "../VoxelizationRequest.h"
#include "../../../context/ApplicationContext.h"
#include "../../../enum/LevelOfDetail.h"
#include "../../../enum/engine-definitions.h"
#include "../../../service/texture/TextureData.h"
#include "../../../service/mesh/MeshData.h"
#include "../../../service/voxel/impl/SparseVoxelOctreeBuilder.h"
#include "RTTriangle.h"

#define MAX_DEPTH 12

namespace Metal {
    bool VoxelizerService::isTriangleFlatInAxis(const RTTriangle &triangle) {
        if (triangle.v0.y == triangle.v1.y && triangle.v1.y == triangle.v2.y) {
            return true; // Flat in the Y-axis
        }
        if (triangle.v0.x == triangle.v1.x && triangle.v1.x == triangle.v2.x) {
            return true; // Flat in the X-axis
        }
        if (triangle.v0.z == triangle.v1.z && triangle.v1.z == triangle.v2.z) {
            return true; // Flat in the Z-axis
        }
        return false;
    }

    void VoxelizerService::iterateTriangle(const MeshComponent *component, const RTTriangle &triangle) {
        const float edgeLength1 = glm::distance(triangle.v0, triangle.v1);
        const float edgeLength2 = glm::distance(triangle.v1, triangle.v2);
        const float edgeLength3 = glm::distance(triangle.v2, triangle.v0);

        const float maxEdgeLength = std::max(edgeLength1, std::max(edgeLength2, edgeLength3));
        int localMaxDepth = MAX_DEPTH;
        const auto voxelSize = static_cast<float>(TILE_SIZE / std::pow(2, MAX_DEPTH));
        float stepSize = voxelSize / maxEdgeLength;
        if (edgeLength1 == INFINITY || edgeLength2 == INFINITY || edgeLength3 == INFINITY) {
            stepSize = .01f;
        }
        //
        // if (isTriangleFlatInAxis(triangle)) {
        //     while (stepSize < .01) {
        //         const auto voxelSizeLocal = static_cast<float>(TILE_SIZE / std::pow(2, localMaxDepth));
        //         stepSize = voxelSizeLocal / maxEdgeLength;
        //         localMaxDepth--;
        //     }
        // }

        glm::vec3 albedo = component->albedoColor * 255.f;
        auto *mat = context.materialService.stream(component->materialId);
        TextureData *albedoData = nullptr;

        if (mat != nullptr) {
            if (!context.voxelizationService.textures.contains(mat->albedo)) {
                albedoData = context.textureService.stream(mat->albedo, LevelOfDetail::LOD_0);
            } else {
                albedoData = context.voxelizationService.textures.at(mat->albedo);
            }
        }

        for (float lambda1 = 0; lambda1 <= 1; lambda1 += stepSize) {
            for (float lambda2 = 0; lambda2 <= 1 - lambda1; lambda2 += stepSize) {
                float lambda0 = 1 - lambda1 - lambda2;
                if (context.voxelizationService.isVoxelizationCancelled) {
                    return;
                }
                glm::vec2 uv = lambda0 * triangle.uv0 + lambda1 * triangle.uv1 + lambda2 * triangle.uv2;
                glm::vec3 point = lambda0 * triangle.v0 +
                                  lambda1 * triangle.v1 +
                                  lambda2 * triangle.v2;
                glm::vec3 normal = glm::normalize(
                    lambda0 * triangle.n0 +
                    lambda1 * triangle.n1 +
                    lambda2 * triangle.n2
                );
                auto *voxelTile = context.worldGridRepository.getTile(point);
                if (voxelTile != nullptr) {
                    if (!context.voxelizationService.builders.contains(voxelTile->id)) {
                        context.voxelizationService.builders.emplace(voxelTile->id,
                                         SparseVoxelOctreeBuilder(voxelTile));
                    }

                    if (albedoData != nullptr) {
                        albedo = glm::vec3(albedoData->sampleAt(uv));
                    }
                    auto ptr = VoxelData(albedo, normal, component->emissiveSurface);
                    context.voxelizationService.builders.at(voxelTile->id).insert(
                        localMaxDepth,
                        point,
                        ptr);
                }
            }
        }
    }

    void VoxelizerService::voxelize(const MeshComponent *component, const glm::mat4x4 &modelMatrix,
                                       const MeshData *mesh) {
        auto &indices = mesh->indices;
        auto &data = mesh->data;
        glm::mat3x3 transposeModel = glm::mat3x3(transpose(glm::inverse(modelMatrix)));
        for (int i = 0; i < indices.size(); i += 3) {
            if (context.voxelizationService.isVoxelizationCancelled) {
                return;
            }
            const auto index = indices[i];
            const auto index1 = indices[i + 1];
            const auto index2 = indices[i + 2];
            auto triangle = RTTriangle(
                modelMatrix * glm::vec4(data[index].vertex, 1),
                modelMatrix * glm::vec4(data[index1].vertex, 1),
                modelMatrix * glm::vec4(data[index2].vertex, 1),

                transposeModel * data[index].normal,
                transposeModel * data[index1].normal,
                transposeModel * data[index2].normal,

                data[index].uv,
                data[index1].uv,
                data[index2].uv
            );

            iterateTriangle(component, triangle);
        }
    }
} // Metal
