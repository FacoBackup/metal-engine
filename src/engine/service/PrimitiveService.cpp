#include <engine/service/PrimitiveService.h>
#include <engine/service/MaterialService.h>
#include <engine/EngineContext.h>
#include <engine/repository/WorldRepository.h>
#include <engine/service/BufferService.h>
#include <engine/resource/BufferInstance.h>
#include <engine/dto/PrimitiveComponent.h>
#include <editor/enum/EngineResourceIDs.h>
#include <engine/frame-builder/EngineFrame.h>

namespace Metal {
    void PrimitiveService::onInitialize() {
        eventListener([this](const Event &) {
            needsUpdate = true;
        }, "PrimitiveComponent");
    }

    void PrimitiveService::onSync() {
        if (needsUpdate) {
            updateMeshMaterials();
            needsUpdate = false;
        }
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
        if (meshMetadata.empty()) return;

        for (auto *frame: engineContext->registeredFrames) {
            auto *meshMetadataBuffer = frame->getResourceAs<BufferInstance>(RID_MESH_METADATA_BUFFER);
            if (meshMetadataBuffer != nullptr) {
                meshMetadataBuffer->update(meshMetadata.data());
            }
        }
    }
}
