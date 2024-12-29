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

        if (stepSize < 0.001) {
            std::cout << "Step size: " << stepSize << " " << edgeLength1 << " " << edgeLength2 << " " << edgeLength3 <<
                    std::endl;
        }
        for (float lambda1 = 0; lambda1 <= 1; lambda1 += stepSize) {
            for (float lambda2 = 0; lambda2 <= 1 - lambda1; lambda2 += stepSize) {
                float lambda0 = 1 - lambda1 - lambda2;

                float u = lambda0 * triangle.uv0.x + lambda1 * triangle.uv1.x + lambda2 * triangle.uv2.x;
                float v = lambda0 * triangle.uv0.y + lambda1 * triangle.uv1.y + lambda2 * triangle.uv2.y;
                glm::vec3 point{
                    lambda0 * triangle.v0.x + lambda1 * triangle.v1.x + lambda2 * triangle.v2.x,
                    lambda0 * triangle.v0.y + lambda1 * triangle.v1.y + lambda2 * triangle.v2.y,
                    lambda0 * triangle.v0.z + lambda1 * triangle.v1.z + lambda2 * triangle.v2.z
                };
                glm::vec3 normal{
                    lambda0 * triangle.n0.x + lambda1 * triangle.n1.x + lambda2 * triangle.n2.x,
                    lambda0 * triangle.n0.y + lambda1 * triangle.n1.y + lambda2 * triangle.n2.y,
                    lambda0 * triangle.n0.z + lambda1 * triangle.n1.z + lambda2 * triangle.n2.z
                };
                auto *voxelTile = context.worldGridRepository.getTile(point);
                if (voxelTile != nullptr) {
                    if (!builders.contains(voxelTile->id)) {
                        builders.emplace(voxelTile->id,
                                         SparseVoxelOctreeBuilder(MAX_DEPTH, voxelTile));
                    }
                    glm::vec3 albedo(component->albedoColor);
                    glm::vec2 roughnessMetallic(component->roughnessFactor, component->metallicFactor);
                    builders.at(voxelTile->id).insert(
                        point, std::make_shared<VoxelData>(albedo, normal, roughnessMetallic,
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

    void VoxelizationService::FillStorage(unsigned int targetDepth, unsigned int &bufferIndex,
                                          unsigned int &materialBufferIndex,
                                          SparseVoxelOctreeData &voxels, OctreeNode *node) {
        if (node->isLeaf(targetDepth)) {
            return;
        }

        voxels.data[node->dataIndex] = node->packVoxelData(bufferIndex, targetDepth);
        bool isParentOfLeaf = true;
        for (auto &child: node->children) {
            if (child != nullptr && !child->isLeaf(targetDepth)) {
                PutData(bufferIndex, child.get());
                isParentOfLeaf = false;
            }
        }

        for (auto &child: node->children) {
            if (child != nullptr) {
                FillStorage(targetDepth, bufferIndex, materialBufferIndex, voxels, child.get());
            }
        }

        if (isParentOfLeaf && node->data != nullptr) {
            std::array<unsigned int, 2> compressed = node->data->compress();
            voxels.data[node->dataIndex] = node->packVoxelData(materialBufferIndex, targetDepth);
            voxels.materialData[materialBufferIndex] = compressed[0];
            materialBufferIndex++;
            voxels.materialData[materialBufferIndex] = compressed[1];
            materialBufferIndex++;
        }
    }

    /**
     * Non-leaf nodes store 16 bit pointer to child group + 8 bit child mask + 8 bit leaf mask
     * @param bufferIndex current buffer index
     * @param node
     */
    void VoxelizationService::PutData(unsigned int &bufferIndex, OctreeNode *node) {
        node->dataIndex = bufferIndex;
        bufferIndex++;
    }

    void VoxelizationService::serialize(SparseVoxelOctreeBuilder &builder) const {
        for (LevelOfDetail &lod: LevelOfDetail::LODs) {
            METRIC_START(std::string("Starting serialization " + builder.getTile()->id))

            const unsigned int desiredMaxDepth = MAX_DEPTH - (lod.level - 1);
            SparseVoxelOctreeData data{};
            data.data.resize(builder.getVoxelQuantity());
            data.materialData.resize(builder.getLeafVoxelQuantity() * 2);

            unsigned int bufferIndex = 0;
            unsigned int materialBufferIndex = 0;
            PutData(bufferIndex, &builder.getRoot());
            FillStorage(desiredMaxDepth, bufferIndex, materialBufferIndex, data, &builder.getRoot());

            std::string filePath = context.getAssetDirectory() + FORMAT_FILE_SVO(builder.getTile()->id, lod);
            DUMP_TEMPLATE(filePath, data)
            METRIC_END("Ending serialization ")
        }
    }

    void VoxelizationService::voxelizeScene() const {
        METRIC_START("Starting Voxelization ")
        std::unordered_map<std::string, SparseVoxelOctreeBuilder> builders;
        for (auto &t: context.worldGridRepository.getTiles()) {
            for (auto entity: t.second.entities) {
                if (context.worldRepository.meshes.contains(entity)) {
                    auto &meshComponent = context.worldRepository.meshes.at(entity);
                    auto *mesh = context.meshService.stream(meshComponent.meshId, LevelOfDetail::LOD_0);
                    if (mesh != nullptr) {
                        auto &transformComponent = context.worldRepository.transforms.at(entity);
                        METRIC_START(std::string("Voxelizing mesh " + meshComponent.getEntityId()))
                        voxelize(&meshComponent, transformComponent.model, mesh, builders);
                        METRIC_END("Voxelization took: ")
                    }
                    delete mesh;
                }
            }
        }

        for (auto &t: builders) {
            serialize(t.second);
        }
        context.svoService.disposeAll();
        METRIC_END("Ending Voxelization ")
    }
} // Metal
