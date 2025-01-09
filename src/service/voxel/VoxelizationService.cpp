#include "VoxelizationService.h"

#include <cereal/archives/binary.hpp>

#include "../../context/ApplicationContext.h"
#include "../../enum/LevelOfDetail.h"
#include "../../enum/engine-definitions.h"
#include "../../service/mesh/MeshData.h"
#include "../../service/voxel/impl/SparseVoxelOctreeBuilder.h"
#include "impl/SparseVoxelOctreeData.h"
#include "impl/Triangle.h"

#define MAX_DEPTH 12

namespace Metal {
    void VoxelizationService::iterateTriangle(const MeshComponent *component, const Triangle &triangle,
                                              std::unordered_map<std::string, SparseVoxelOctreeBuilder> &builders)
    const {
        const float edgeLength1 = glm::distance(triangle.v0, triangle.v1);
        const float edgeLength2 = glm::distance(triangle.v1, triangle.v2);
        const float edgeLength3 = glm::distance(triangle.v2, triangle.v0);

        const float maxEdgeLength = std::max(edgeLength1, std::max(edgeLength2, edgeLength3));
        const auto voxelSize = static_cast<float>(TILE_SIZE / std::pow(2, MAX_DEPTH));
        float stepSize = voxelSize / maxEdgeLength;
        if (edgeLength1 == INFINITY || edgeLength2 == INFINITY || edgeLength3 == INFINITY) {
            stepSize = .01f;
        }

        glm::vec3 albedo = component->albedoColor * 255.f;
        glm::vec2 roughnessMetallic(component->roughnessFactor, component->metallicFactor);

        for (float lambda1 = 0; lambda1 <= 1; lambda1 += stepSize) {
            for (float lambda2 = 0; lambda2 <= 1 - lambda1; lambda2 += stepSize) {
                float lambda0 = 1 - lambda1 - lambda2;

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
                    if (!builders.contains(voxelTile->id)) {
                        builders.emplace(voxelTile->id,
                                         SparseVoxelOctreeBuilder(voxelTile));
                    }
                    builders.at(voxelTile->id).insert(
                        MAX_DEPTH,
                        point,
                        new VoxelData(
                            albedo, normal, roughnessMetallic,
                            component->emissiveSurface));
                }
            }
        }
    }

    void VoxelizationService::voxelize(const MeshComponent *component, const glm::mat4x4 &modelMatrix,
                                       const MeshData *mesh,
                                       std::unordered_map<std::string, SparseVoxelOctreeBuilder> &builders) const {
        auto &indices = mesh->indices;
        auto &data = mesh->data;

        for (int i = 0; i < indices.size(); i += 3) {
            const auto index = indices[i];
            const auto index1 = indices[i + 1];
            const auto index2 = indices[i + 2];
            auto triangle = Triangle(
                modelMatrix * glm::vec4(data[index].vertex, 1),
                modelMatrix * glm::vec4(data[index1].vertex, 1),
                modelMatrix * glm::vec4(data[index2].vertex, 1),

                data[index].normal,
                data[index1].normal,
                data[index2].normal,

                data[index].uv,
                data[index1].uv,
                data[index2].uv
            );

            iterateTriangle(component, triangle, builders);
        }
    }

    void VoxelizationService::FillStorage(SparseVoxelOctreeBuilder &builder, unsigned int &bufferIndex,
                                          unsigned int &materialBufferIndex,
                                          SparseVoxelOctreeData &voxels, OctreeNode *node) {
        if (node->isLeaf) {
            return;
        }

        // if (node->isStructureStart) {
        //     auto *newNode = builder.repeatedStructures.at(node->getId());
        //     if (!newNode->processed) {
        //         PutData(voxels, bufferIndex, newNode);
        //         FillStorage(builder, bufferIndex, voxels, newNode);
        //     }
        //     voxels.data[node->dataIndex] = voxels.data[newNode->dataIndex]; // POINT THIS VOXEL TO THE BEGINNING OF THE STRUCTURE
        //     return;
        // }

        voxels.data[node->dataIndex] = node->packVoxelData(bufferIndex);
        bool isParentOfLeaf = true;
        for (auto &child: node->children) {
            if (child != nullptr && !child->isLeaf) {
                PutData(bufferIndex, child);
                isParentOfLeaf = false;
            }
        }

        for (auto &child: node->children) {
            if (child != nullptr) {
                FillStorage(builder, bufferIndex, materialBufferIndex, voxels, child);
            }
        }

        if (isParentOfLeaf && node->data != nullptr) {
            std::array<unsigned int, 2> &compressed = node->data->data;
            voxels.data[node->dataIndex] = node->packVoxelData(materialBufferIndex);
            voxels.data[materialBufferIndex + voxels.voxelBufferOffset] = compressed[0];
            materialBufferIndex++;
            voxels.data[materialBufferIndex + voxels.voxelBufferOffset] = compressed[1];
            materialBufferIndex++;
        }
    }

    void VoxelizationService::PutData(unsigned int &bufferIndex, OctreeNode *node) {
        node->dataIndex = bufferIndex;
        bufferIndex++;
    }

    void VoxelizationService::serialize(SparseVoxelOctreeBuilder &builder) const {
        // builder.findRepeatedStructures();

        METRIC_START
        SparseVoxelOctreeData data{};
        data.data.resize(builder.getVoxelQuantity() + builder.getLeafVoxelQuantity() * 2);
        data.voxelBufferOffset = builder.getVoxelQuantity();

        unsigned int bufferIndex = 0;
        unsigned int materialBufferIndex = 0;
        PutData(bufferIndex, &builder.getRoot());
        FillStorage(builder, bufferIndex, materialBufferIndex, data, &builder.getRoot());

        std::string filePath = context.getAssetDirectory() + FORMAT_FILE_SVO(builder.getTile()->id);
        context.engineRepository.svoFilePaths.push_back(filePath);
        DUMP_TEMPLATE(filePath, data)
        METRIC_END("Ending serialization ")
    }

    void VoxelizationService::voxelizeScene() const {
        METRIC_START
        for (auto path: context.engineRepository.svoFilePaths) {
            if (std::filesystem::exists(path)) {
                std::filesystem::remove(path);
            }
        }
        context.engineRepository.svoFilePaths.clear();
        std::unordered_map<std::string, SparseVoxelOctreeBuilder> builders;
        for (auto &t: context.worldGridRepository.getTiles()) {
            for (auto entity: t.second.entities) {
                if (context.worldRepository.meshes.contains(entity)) {
                    auto &meshComponent = context.worldRepository.meshes.at(entity);
                    auto &transformComponent = context.worldRepository.transforms.at(entity);
                    if (transformComponent.isStatic) {
                        auto *mesh = context.meshService.stream(meshComponent.meshId, LevelOfDetail::LOD_0);
                        if (mesh != nullptr) {
                            METRIC_START
                            voxelize(&meshComponent, transformComponent.model, mesh, builders);
                            METRIC_END(std::format("Voxelization of mesh {}", meshComponent.getEntityId()))
                        }
                        delete mesh;
                    }
                }
            }
        }

        for (auto &t: builders) {
            serialize(t.second);
            t.second.dispose();
        }

        context.svoService.disposeAll();
        context.worldGridRepository.hasMainTileChanged = true;
        context.engineContext.setGISettingsUpdated(true);
        METRIC_END("Ending Voxelization ")
    }
} // Metal
