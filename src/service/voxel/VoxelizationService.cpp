#include "VoxelizationService.h"

#include <stb_image.h>
#include <thread>
#include <cereal/archives/binary.hpp>

#include "VoxelizationRequest.h"
#include "../../context/ApplicationContext.h"
#include "../../enum/LevelOfDetail.h"
#include "../../enum/engine-definitions.h"
#include "../../service/texture/TextureData.h"
#include "../../service/mesh/MeshData.h"
#include "../../service/voxel/impl/SparseVoxelOctreeBuilder.h"
#include "impl/SparseVoxelOctreeData.h"
#include "impl/Triangle.h"

#define MAX_DEPTH 12

namespace Metal {
    void VoxelizationService::iterateTriangle(const MeshComponent *component, const Triangle &triangle) {
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
        auto *mat = context.materialService.stream(component->materialId);
        TextureData *albedoData = nullptr;

        if (mat != nullptr) {
            if (!textures.contains(mat->albedo)) {
                albedoData = context.textureService.stream(mat->albedo, LevelOfDetail::LOD_0);
            } else {
                albedoData = textures.at(mat->albedo);
            }
        }

        for (float lambda1 = 0; lambda1 <= 1; lambda1 += stepSize) {
            for (float lambda2 = 0; lambda2 <= 1 - lambda1; lambda2 += stepSize) {
                float lambda0 = 1 - lambda1 - lambda2;
                if (isVoxelizationCancelled) {
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
                    if (!builders.contains(voxelTile->id)) {
                        builders.emplace(voxelTile->id,
                                         SparseVoxelOctreeBuilder(voxelTile));
                    }

                    if (albedoData != nullptr) {
                        albedo = glm::vec3(albedoData->sampleAt(uv));
                    }
                    auto ptr = VoxelData(albedo, normal, component->emissiveSurface);
                    builders.at(voxelTile->id).insert(
                        MAX_DEPTH,
                        point,
                        ptr);
                }
            }
        }
    }

    void VoxelizationService::voxelize(const MeshComponent *component, const glm::mat4x4 &modelMatrix,
                                       const MeshData *mesh) {
        auto &indices = mesh->indices;
        auto &data = mesh->data;
        glm::mat3x3 transposeModel = glm::mat3x3(transpose(glm::inverse(modelMatrix)));
        for (int i = 0; i < indices.size(); i += 3) {
            if (isVoxelizationCancelled) {
                return;
            }
            const auto index = indices[i];
            const auto index1 = indices[i + 1];
            const auto index2 = indices[i + 2];
            auto triangle = Triangle(
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

        if (isParentOfLeaf) {
            std::array<unsigned int, 2> &compressed = node->data;
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

    void VoxelizationService::voxelizeGroup(const std::vector<VoxelizationRequest> &request) {
        for (auto &localRequest: request) {
            METRIC_START
            auto *mesh = context.meshService.stream(localRequest.meshComponent.meshId, LevelOfDetail::LOD_0);
            if (mesh != nullptr) {
                voxelize(&localRequest.meshComponent, localRequest.transform, mesh);
                delete mesh;
            }
            METRIC_END(std::format("Voxelization of mesh {}", localRequest.meshComponent.getEntityId()))
        }
    }

    void VoxelizationService::collectRequests(WorldTile &t,
                                              std::array<std::vector<VoxelizationRequest>, 3> &requests) const {
        unsigned int requestIndex = 0;
        for (auto entity: t.entities) {
            if (context.worldRepository.meshes.contains(entity) && !context.worldRepository.hiddenEntities.contains(entity)) {
                auto &meshComponent = context.worldRepository.meshes.at(entity);
                auto &transformComponent = context.worldRepository.transforms.at(entity);
                if (transformComponent.isStatic) {
                    requests[requestIndex].emplace_back(transformComponent.model, meshComponent);
                    requestIndex++;
                    if (requestIndex >= requests.size()) {
                        requestIndex = 0;
                    }
                }
            }
        }
    }

    void VoxelizationService::voxelize() {
        METRIC_START
        for (auto path: context.engineRepository.svoFilePaths) {
            if (std::filesystem::exists(path)) {
                std::filesystem::remove(path);
            }
        }
        context.engineRepository.svoFilePaths.clear();
        for (auto &t: context.worldGridRepository.getTiles()) {
            std::array<std::vector<VoxelizationRequest>, 3> requests;
            collectRequests(t.second, requests);

            if (!isVoxelizationCancelled) {
                std::vector<std::thread> threads;
                for (int i = 0; i < requests.size(); i++) {
                    auto &request = requests[i];
                    if (i == requests.size() - 1) {
                        voxelizeGroup(request);
                    } else {
                        threads.emplace_back([this, request]() {
                            voxelizeGroup(request);
                        });
                    }
                }
                for (auto &t: threads) {
                    if (t.joinable()) {
                        t.join();
                    }
                }
            }
        }

        for (auto &t: builders) {
            serialize(t.second);
            t.second.dispose();
        }

        for (auto &entry: textures) {
            stbi_image_free(entry.second->data);
            delete entry.second;
        }
        textures.clear();
        builders.clear();
        METRIC_END("Ending Voxelization ")

        isVoxelizationDone = true;
    }

    void VoxelizationService::onSync() {
        if (isVoxelizationDone) {
            std::cout << "Voxelization was finished; Clearing all resources affected" << std::endl;
            context.worldGridRepository.hasMainTileChanged = true;
            context.svoService.disposeAll();
            context.engineContext.setGISettingsUpdated(true);
            isVoxelizationDone = false;
            isExecutingThread = false;
            context.notificationService.pushMessage("Voxelization done", NotificationSeverities::SUCCESS);

            if (thread.joinable()) {
                thread.join();
            }
        }
        if (localVoxelizationRequestId == context.engineContext.getVoxelizationRequestId()) {
            return;
        }
        localVoxelizationRequestId = context.engineContext.getVoxelizationRequestId();

        // New voxelization request; Cancels the previous one and starts all over again
        cancelRequest();
        context.notificationService.pushMessage("Voxelization started", NotificationSeverities::WARNING);
        std::cout << "Dispatching new voxelization request" << std::endl;
        isExecutingThread = true;
        if (thread.joinable()) {
            thread.join(); // Wait for the thread to finish
        }
        thread = std::thread([this]() {
            voxelize();
        });
    }

    void VoxelizationService::cancelRequest() {
        if (isExecutingThread) {
            std::cout << "Cancelling previous voxelization request" << std::endl;
            isVoxelizationCancelled = true;
            thread.join();
            isVoxelizationCancelled = false;
            context.notificationService.pushMessage("Voxelization cancelled", NotificationSeverities::SUCCESS);
        }
    }
} // Metal
