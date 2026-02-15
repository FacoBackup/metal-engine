#include "VoxelizationService.h"

#include <stb_image.h>
#include <thread>
#include <cereal/archives/binary.hpp>

#include "VoxelizationRequest.h"
#include "../../context/ApplicationContext.h"
#include "../../service/log/LogService.h"
#include "../../enum/LevelOfDetail.h"
#include "../../enum/engine-definitions.h"
#include "../../service/texture/TextureData.h"
#include "../../service/mesh/MeshData.h"
#include "../../service/voxel/impl/SparseVoxelOctreeBuilder.h"
#include "impl/SparseVoxelOctreeData.h"
#include "impl/SnapshotWorldTile.h"
#include <functional>

#define METRIC_START \
auto currentTime = Clock::now(); \
auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime.time_since_epoch());\
const auto start = duration.count();

#define METRIC_END(M) \
currentTime = Clock::now();\
duration = std::chrono::duration_cast< std::chrono::milliseconds>(currentTime.time_since_epoch());\
std::cout << M << " " << duration.count() - start << "ms" << std::endl;


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

    void VoxelizationService::collectRequests(const SnapshotWorldTile &t,
                                              std::vector<std::vector<VoxelizationRequest> > &requests) const {
        unsigned int requestIndex = 0;
        if (!worldSnapshot->entitiesByTile.contains(t.id)) return;
        for (auto &entity: worldSnapshot->entitiesByTile.at(t.id)) {
            requests[requestIndex].emplace_back(entity.model, *entity.meshComponent);
            requestIndex++;
            if (requestIndex >= requests.size()) {
                requestIndex = 0;
            }
        }
    }

    VoxelizationService::VoxelizationService(ApplicationContext &context) : AbstractRuntimeComponent(context) {
        worldSnapshot = new WorldSnapshot();
    }

    void VoxelizationService::serialize(SparseVoxelOctreeBuilder &builder, const std::string &filePath) const {
        METRIC_START
        SparseVoxelOctreeData data{};
        data.data.resize(builder.getVoxelQuantity() + builder.getLeafVoxelQuantity() * 2);
        data.voxelBufferOffset = builder.getVoxelQuantity();

        unsigned int bufferIndex = 0;
        unsigned int materialBufferIndex = 0;
        PutData(bufferIndex, &builder.getRoot());
        FillStorage(builder, bufferIndex, materialBufferIndex, data, &builder.getRoot());

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

        for (auto path: context.engineRepository.svoFilePaths) {
            if (std::filesystem::exists(path)) {
                std::filesystem::remove(path);
            }
        }
        context.engineRepository.svoFilePaths.clear();

        for (auto &t: worldSnapshot->tiles) {
            const unsigned int threadCount = std::max(1u, std::thread::hardware_concurrency());
            std::vector<std::vector<VoxelizationRequest> > requests;
            requests.resize(threadCount);
            collectRequests(t.second, requests);

            if (!isVoxelizationCancelled) {
                std::vector<std::thread> threads;
                for (int i = 0; i < requests.size(); i++) {
                    auto &request = requests[i];
                    if (i == requests.size() - 1) {
                        try {
                            voxelizeGroup(request);
                        } catch (std::exception &e) {
                            std::cerr << e.what() << std::endl;
                        }
                    } else {
                        threads.emplace_back([this, request]() {
                            try {
                                voxelizeGroup(request);
                            } catch (std::exception &e) {
                                std::cerr << e.what() << std::endl;
                            }
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
            std::string filePath = context.getAssetDirectory() + FORMAT_FILE_SVO(t.second->getTile()->id);
            context.engineRepository.svoFilePaths.push_back(filePath);
            serialize(*t.second, filePath);
            t.second->dispose();
        }

        for (auto &entry: textures) {
            stbi_image_free(entry.second->data);
            delete entry.second;
        }
        textures.clear();
        builders.clear();
        METRIC_END("Ending Voxelization ")

        if (!voxelizationTask.empty()) {
            context.asyncTaskService.endTask(voxelizationTask, false);
            voxelizationTask = "";
        }

        isVoxelizationDone = true;
    }

    void VoxelizationService::onSync() {
        if (isVoxelizationDone) {
            LOG_INFO(context, "Voxelization was finished; Clearing all resources affected");
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
        captureSnapshot();
        context.notificationService.pushMessage("Voxelization started", NotificationSeverities::WARNING);
        LOG_INFO(context, "Dispatching new voxelization request");
        voxelizationTask = context.asyncTaskService.registerTask(
            "Voxelizing scene",
            [this] {
                cancelRequest();
            });
        isExecutingThread = true;
        try {
            if (mainThread.joinable()) {
                mainThread.join();
            }
        } catch (std::exception &e) {
            std::cerr << e.what() << std::endl;
        }
        mainThread = std::thread([this]() {
            beginVoxelization();
        });
    }

    void VoxelizationService::cancelRequest() {
        if (isExecutingThread) {
            LOG_INFO(context, "Cancelling previous voxelization request");
            isVoxelizationCancelled = true;
            try {
                if (mainThread.joinable()) {
                    mainThread.join();
                }
            } catch (std::exception &e) {
                std::cerr << e.what() << std::endl;
            }
            if (!voxelizationTask.empty()) {
                context.asyncTaskService.endTask(voxelizationTask, false);
                voxelizationTask = "";
            }
            isVoxelizationCancelled = false;
            context.notificationService.pushMessage("Voxelization cancelled", NotificationSeverities::SUCCESS);
        }
    }

    void VoxelizationService::captureSnapshot() {
        worldSnapshot->tiles.clear();
        worldSnapshot->entitiesByTile.clear();
        meshComponentSnapshot.clear();

        for (auto &pair: context.worldGridRepository.getTiles()) {
            auto &tile = pair.second;
            worldSnapshot->tiles.emplace(tile.id, SnapshotWorldTile(tile.x, tile.z, tile.id, tile.boundingBox));

            auto &entitiesInTile = worldSnapshot->entitiesByTile[tile.id];
            for (auto entityId: tile.entities) {
                if (context.worldRepository.meshes.contains(entityId) &&
                    !context.worldRepository.hiddenEntities.contains(entityId)) {
                    auto &meshComp = context.worldRepository.meshes.at(entityId);
                    auto &transComp = context.worldRepository.transforms.at(entityId);

                    if (transComp.isStatic) {
                        auto snapshotMesh = std::make_unique<MeshComponent>();
                        copyMeshComponent(meshComp, *snapshotMesh);

                        SnapshotEntity snapshot;
                        snapshot.model = transComp.model;
                        snapshot.meshComponent = snapshotMesh.get();

                        meshComponentSnapshot.push_back(std::move(snapshotMesh));
                        entitiesInTile.push_back(snapshot);
                    }
                }
            }
        }
    }

    void VoxelizationService::copyMeshComponent(const MeshComponent &from, MeshComponent &to) {
        to.setEntityId(from.getEntityId());
        to.meshId = from.meshId;
        to.materialId = from.materialId;
        to.albedoColor = from.albedoColor;
        to.emissiveSurface = from.emissiveSurface;
        to.roughnessFactor = from.roughnessFactor;
        to.metallicFactor = from.metallicFactor;
        to.parallaxLayers = from.parallaxLayers;
        to.parallaxHeightScale = from.parallaxHeightScale;
        to.needsReVoxelization = from.needsReVoxelization;
    }
} // Metal
