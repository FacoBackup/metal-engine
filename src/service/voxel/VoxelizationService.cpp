#include "VoxelizationService.h"

#include <cereal/archives/binary.hpp>

#include "../../context/ApplicationContext.h"
#include "../../enum/LevelOfDetail.h"
#include "../../enum/engine-definitions.h"
#include "../../service/mesh/MeshData.h"
#include "../../service/voxel/impl/SparseVoxelOctreeBuilder.h"
#include "impl/SparseVoxelOctreeData.h"
#include "impl/Triangle.h"

namespace Metal {
    void VoxelizationService::iterateTriangle(const Triangle &triangle,
                                              std::unordered_map<std::string, SparseVoxelOctreeBuilder> &builders)
    const {
        const float edgeLength1 = glm::distance(triangle.v0, triangle.v1);
        const float edgeLength2 = glm::distance(triangle.v1, triangle.v2);
        const float edgeLength3 = glm::distance(triangle.v2, triangle.v0);

        const float maxEdgeLength = std::max(edgeLength1, std::max(edgeLength2, edgeLength3));
        const auto voxelSize = static_cast<float>(TILE_SIZE / std::pow(2, context.voxelizationRepository.maxDepth));
        const float stepSize = voxelSize / maxEdgeLength;

        for (float lambda1 = 0; lambda1 <= 1; lambda1 += stepSize) {
            for (float lambda2 = 0; lambda2 <= 1 - lambda1; lambda2 += stepSize) {
                float lambda0 = 1 - lambda1 - lambda2;

                // float u = lambda0 * triangle.uv0.x + lambda1 * triangle.uv1.x + lambda2 * triangle.uv2.x;
                // float v = lambda0 * triangle.uv0.y + lambda1 * triangle.uv1.y + lambda2 * triangle.uv2.y;
                glm::vec3 point{
                    lambda0 * triangle.v0.x + lambda1 * triangle.v1.x + lambda2 * triangle.v2.x,
                    lambda0 * triangle.v0.y + lambda1 * triangle.v1.y + lambda2 * triangle.v2.y,
                    lambda0 * triangle.v0.z + lambda1 * triangle.v1.z + lambda2 * triangle.v2.z
                };
                auto *voxelTile = context.worldGridRepository.getOrCreateTile(point);
                if (!builders.contains(voxelTile->id)) {
                    builders.emplace(voxelTile->id,
                                     SparseVoxelOctreeBuilder(context.voxelizationRepository.maxDepth, voxelTile));
                }
                builders.at(voxelTile->id).insert(point, std::make_shared<VoxelData>(glm::vec3{255, 255, 255}));
            }
        }
    }

    void VoxelizationService::voxelize(const MeshData *mesh,
                                       std::unordered_map<std::string, SparseVoxelOctreeBuilder> &builders) const {
        auto &indices = mesh->indices;
        auto &data = mesh->data;

        for (int i = 0; i < indices.size(); i += 3) {
            const auto index = indices[i];
            const auto index1 = indices[i + 1];
            const auto index2 = indices[i + 2];
            auto triangle = Triangle(
                data[index * 3].vertex,
                data[index1 * 3].vertex,
                data[index2 * 3].vertex
            );

            triangle.uv0 = data[index * 2].uv;
            triangle.uv1 = data[index1 * 2].uv;
            triangle.uv2 = data[index2 * 2].uv;

            iterateTriangle(triangle, builders);
        }
    }

    void VoxelizationService::FillStorage(unsigned int &bufferIndex, SparseVoxelOctreeData &voxels, OctreeNode *node) {
        if (node->isLeaf) {
            return;
        }

        voxels.data[node->dataIndex] = node->packVoxelData(bufferIndex);
        bool isParentOfLeaf = true;
        for (auto &child: node->children) {
            if (child != nullptr && !child->isLeaf) {
                PutData(bufferIndex, child.get());
                isParentOfLeaf = false;
            }
        }

        for (auto &child: node->children) {
            if (child != nullptr) {
                FillStorage(bufferIndex, voxels, child.get());
            }
        }
        if (isParentOfLeaf && node->data != nullptr) {
            voxels.data[node->dataIndex] = node->packVoxelData(node->data->compress());
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
        auto currentTime = Clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime.time_since_epoch());
        const auto start = duration.count();

        SparseVoxelOctreeData data{};
        std::cout << "Serializing voxel data for " << builder.getTile()->id << std::endl;
        std::cout << "Voxel quantity: " << builder.getVoxelQuantity() << std::endl;
        data.data.reserve(builder.getVoxelQuantity());

        unsigned int bufferIndex = 0;
        PutData(bufferIndex, &builder.getRoot());
        FillStorage(bufferIndex, data, &builder.getRoot());

        DUMP_TEMPLATE(context.getAssetDirectory() + FORMAT_FILE_SVO(builder.getTile()->id), data)

        currentTime = Clock::now();
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime.time_since_epoch());
        std::cout << "Serialization took: " << start - duration.count() << "ms" << std::endl;
    }

    void VoxelizationService::voxelizeScene() const {
        std::cout << "Starting scene voxelization" << std::endl;
        std::unordered_map<std::string, SparseVoxelOctreeBuilder> builders;
        for (auto &t: context.worldGridRepository.getTiles()) {
            for (auto entity: t.second.entities) {
                if (context.worldRepository.meshes.contains(entity)) {
                    auto &meshComponent = context.worldRepository.meshes.at(entity);
                    auto *mesh = context.meshService.stream(meshComponent.meshId, LevelOfDetail::LOD_0);
                    if (mesh != nullptr) {
                        auto currentTime = Clock::now();
                        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime.time_since_epoch());
                        const auto start = duration.count();


                        std::cout << "Voxelizing mesh " << meshComponent.getEntityId() << std::endl;
                        voxelize(mesh, builders);

                        currentTime = Clock::now();
                        duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime.time_since_epoch());
                        std::cout << "Voxelization took: " << start - duration.count() << "ms" << std::endl;
                    }
                    delete mesh;
                }
            }
        }

        for (auto &t: builders) {
            serialize(t.second);
        }
    }
} // Metal