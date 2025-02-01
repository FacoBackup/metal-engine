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

namespace Metal {
    void VoxelizationService::FillStorage(SparseVoxelOctreeBuilder &builder, unsigned int &bufferIndex,
                                          unsigned int &materialBufferIndex,
                                          SparseVoxelOctreeData &voxels, OctreeNode *node) {
        if (node->isLeaf) {
            return;
        }

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

    void VoxelizationService::collectRequests(WorldTile &t,
                                              std::vector<std::vector<VoxelizationRequest> > &requests) const {
        unsigned int requestIndex = 0;
        for (auto entity: t.entities) {
            if (context.worldRepository.meshes.contains(entity) && !context.worldRepository.hiddenEntities.
                contains(entity)) {
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

    void VoxelizationService::serialize(SparseVoxelOctreeBuilder &builder) const {
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

    void VoxelizationService::voxelizeGroup(const std::vector<VoxelizationRequest> &request) const {
        for (auto &localRequest: request) {
            METRIC_START
            auto *mesh = context.meshService.stream(localRequest.meshComponent.meshId, LevelOfDetail::LOD_0);
            if (mesh != nullptr) {
                context.voxelizerService.voxelize(&localRequest.meshComponent, localRequest.transform, mesh);
                delete mesh;
            }
            METRIC_END(std::format("Voxelization of mesh {}", localRequest.meshComponent.getEntityId()))
        }
    }

    void VoxelizationService::beginVoxelization() {
        METRIC_START
        // std::filesystem::recursive_directory_iterator iter(context.getAssetDirectory());
        // for (const auto &entry: std::filesystem::recursive_directory_iterator(context.getAssetDirectory())) {
        //     std::string fileName = entry.path().filename().string();
        //     if (entry.is_regular_file() &&
        //         fileName.find(std::format("{}{}",FILE_NAME_SEPARATOR, FILE_SVO)) !=
        //         std::string::npos) {
        //         EntityID id = std::stoi(fileName.substr(0, fileName.find_last_of(FILE_NAME_SEPARATOR)));
        //         auto *entityFound = context.worldRepository.getEntity(id);
        //         if (entityFound == nullptr || // Entity doesnt exist
        //             Util::indexOf(entityFound->components, ComponentTypes::MESH) == -1 || // Component doesnt exist
        //             context.worldRepository.meshes.at(id).needsReVoxelization // Re-voxelize
        //         ) {
        //             std::filesystem::remove(entry.path());
        //         }
        //     }
        // }

        for (auto path: context.engineRepository.svoFilePaths) {
            if (std::filesystem::exists(path)) {
                std::filesystem::remove(path);
            }
        }
        context.engineRepository.svoFilePaths.clear();

        for (auto &t: context.worldGridRepository.getTiles()) {
            std::vector<std::vector<VoxelizationRequest> > requests;
            requests.resize(1); // TODO - FIX FOR MULTIPLE THREADS
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

            if (mainThread.joinable()) {
                mainThread.join();
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
        if (mainThread.joinable()) {
            mainThread.join(); // Wait for the thread to finish
        }
        mainThread = std::thread([this]() {
            beginVoxelization();
        });
    }

    void VoxelizationService::cancelRequest() {
        if (isExecutingThread) {
            std::cout << "Cancelling previous voxelization request" << std::endl;
            isVoxelizationCancelled = true;
            if (mainThread.joinable()) {
                mainThread.join();
            }
            isVoxelizationCancelled = false;
            context.notificationService.pushMessage("Voxelization cancelled", NotificationSeverities::SUCCESS);
        }
    }
} // Metal
