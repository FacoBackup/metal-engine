#include <engine/service/PrimitiveService.h>
#include <engine/service/MaterialService.h>
#include <engine/service/BLASService.h>
#include <engine/EngineContext.h>
#include <engine/repository/WorldRepository.h>
#include <engine/service/BufferService.h>
#include <engine/service/DirtyStateService.h>
#include <ApplicationEventContext.h>
#include <editor/dto/InspectableEventPayload.h>
#include <engine/resource/BufferInstance.h>
#include <engine/dto/PrimitiveComponent.h>
#include <editor/enum/EngineResourceIDs.h>
#include <engine/frame-builder/EngineFrame.h>
#include <engine/resource/BLASInstance.h>
#include <editor/enum/engine-definitions.h>
#include <core/VulkanContext.h>
#include <common/LoggerUtil.h>

#include "engine/dto/MeshData.h"

namespace Metal {
    void PrimitiveService::onInitialize() {
        eventListener([this](const Event &e) {
            const auto payload = std::static_pointer_cast<InspectableEventPayload>(e.payload);
            if (const auto primitive = dynamic_cast<PrimitiveComponent *>(payload->inspectable)) {
                dirtyStateService->markEntityDirty(primitive->getEntityId(), DirtyType::Material);
            }
        }, "PrimitiveComponent");

        eventListener([this](const Event &) {
            fullRebuildNeeded = true;
        }, "BVHUpdated");

        for (auto entity: worldRepository->registry.view<PrimitiveComponent>()) {
            dirtyStateService->markEntityDirty(entity, DirtyType::Material);
        }
    }

    void PrimitiveService::onSync() {
        bool changed = false;

        if (fullRebuildNeeded) {
            changed |= rebuildMetadata();
        }

        changed |= processDirtyMaterials();

        if (changed) {
            uploadMetadata();
        }
    }

    bool PrimitiveService::rebuildMetadata() {
        LOG_INFO("Rebuilding mesh metadata...");
        fullRebuildNeeded = false;
        meshMetadata.clear();

        auto view = worldRepository->registry.view<PrimitiveComponent>();
        uint32_t maxIndex = 0;
        for (auto entity: view) {
            if (worldRepository->hiddenEntities.contains(entity)) continue;
            auto &meshComp = view.get<PrimitiveComponent>(entity);
            if (meshComp.renderIndex != 0xFFFFFFFF && meshComp.renderIndex >= maxIndex) {
                maxIndex = meshComp.renderIndex + 1;
            }
        }

        if (maxIndex == 0) {
            LOG_DEBUG("No meshes found for metadata rebuild.");
            return false;
        }

        LOG_DEBUG("Resizing mesh metadata to " + std::to_string(maxIndex));
        meshMetadata.resize(maxIndex);
        bool changed = false;

        for (auto entity: view) {
            if (worldRepository->hiddenEntities.contains(entity)) continue;
            auto &meshComp = view.get<PrimitiveComponent>(entity);
            if (meshComp.meshId.empty() || meshComp.renderIndex == 0xFFFFFFFF) continue;

            auto *blas = blasService->buildBLAS(meshComp.meshId);
            if (blas && blas->vertexData && blas->indexData) {
                auto &meta = meshMetadata[meshComp.renderIndex];
                meta.renderIndex = meshComp.renderIndex;

                VkBufferDeviceAddressInfo vertexInfo{VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO};
                vertexInfo.buffer = blas->vertexData->vkBuffer;
                meta.vertexBufferAddress = vulkanContext->vkGetBufferDeviceAddressKHR(
                    vulkanContext->device.device, &vertexInfo);

                VkBufferDeviceAddressInfo indexInfo{VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO};
                indexInfo.buffer = blas->indexData->vkBuffer;
                meta.indexBufferAddress = vulkanContext->vkGetBufferDeviceAddressKHR(
                    vulkanContext->device.device, &indexInfo);

                materialService->load(meta, meshComp);
                changed = true;
            }
        }
        return changed;
    }

    bool PrimitiveService::processDirtyMaterials() {
        bool changed = false;
        auto dirtyEntities = dirtyStateService->getDirtyEntities(DirtyType::Material, true);

        if (!dirtyEntities.empty()) {
            LOG_DEBUG("Processing " + std::to_string(dirtyEntities.size()) + " dirty material entities.");
        }

        for (auto entity: dirtyEntities) {
            if (!worldRepository->registry.valid(entity)) continue;
            if (worldRepository->hiddenEntities.contains(entity)) continue;
            auto &meshComp = worldRepository->registry.get<PrimitiveComponent>(entity);
            if (meshComp.meshId.empty() || meshComp.renderIndex == 0xFFFFFFFF) continue;

            if (meshComp.renderIndex < meshMetadata.size()) {
                materialService->load(meshMetadata[meshComp.renderIndex], meshComp);
                changed = true;
            } else {
                LOG_WARN("Dirty entity " + std::to_string(static_cast<uint32_t>(entity)) +
                         " has renderIndex out of bounds (" + std::to_string(meshComp.renderIndex) +
                         " >= " + std::to_string(meshMetadata.size()) + ")");
            }
        }
        return changed;
    }

    void PrimitiveService::updateMeshMaterials() {
        bool changed = false;
        auto view = worldRepository->registry.view<PrimitiveComponent>();

        for (auto entity: view) {
            if (worldRepository->hiddenEntities.contains(entity)) continue;
            auto &meshComp = view.get<PrimitiveComponent>(entity);
            if (meshComp.meshId.empty()) continue;

            if (meshComp.renderIndex < meshMetadata.size()) {
                materialService->load(meshMetadata[meshComp.renderIndex], meshComp);
                changed = true;
            }
        }

        if (changed) {
            uploadMetadata();
        }
    }

    void PrimitiveService::addMetadata(const MeshMetadata &metadata) {
        meshMetadata.push_back(metadata);
    }

    void PrimitiveService::clearMetadata() {
        meshMetadata.clear();
    }

    void PrimitiveService::uploadMetadata() {
        if (meshMetadata.empty()) {
            LOG_TRACE("No metadata to upload.");
            return;
        }

        LOG_DEBUG("Uploading " + std::to_string(meshMetadata.size()) + " mesh metadata entries to GPU.");
        for (auto *frame: engineContext->registeredFrames) {
            auto *meshMetadataBuffer = frame->getResourceAs<BufferInstance>(RID_MESH_METADATA_BUFFER);
            if (meshMetadataBuffer != nullptr) {
                meshMetadataBuffer->update(meshMetadata.data(), meshMetadata.size() * sizeof(MeshMetadata));
            }
        }
    }
}
