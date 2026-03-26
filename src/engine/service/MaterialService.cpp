#include "MaterialService.h"

#include "../../common/serialization-definitions.h"
#include "../../common/LoggerUtil.h"
#include "../dto/MaterialData.h"
#include "TextureService.h"
#include "../repository/WorldRepository.h"
#include "../dto/PrimitiveComponent.h"
#include "../dto/TransformComponent.h"
#include "../EngineContext.h"
#include "BufferService.h"
#include "DirtyStateService.h"
#include "../resource/BufferInstance.h"
#include "../../editor/enum/EngineResourceIDs.h"

namespace Metal {

    void MaterialService::onInitialize() {
        eventListener([this](const Event &) {
            fullRebuildNeeded = true;
        }, "PrimitiveComponent");

        eventListener([this](const Event &) {
            fullRebuildNeeded = true;
        }, "BVHUpdated");
    }

    void MaterialService::onSync() {
        bool changed = false;

        if (fullRebuildNeeded) {
            changed |= rebuildMaterialData();
        }

        changed |= processDirtyMaterials();

        if (changed) {
            uploadMaterialData();
        }
    }

    bool MaterialService::rebuildMaterialData() {
        LOG_INFO("Rebuilding material data...");
        fullRebuildNeeded = false;
        materialData.clear();

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
            LOG_DEBUG("No meshes found for material data rebuild.");
            return false;
        }

        LOG_DEBUG("Resizing material data to " + std::to_string(maxIndex));
        materialData.resize(maxIndex);
        bool changed = false;

        for (auto entity: view) {
            if (worldRepository->hiddenEntities.contains(entity)) continue;
            auto &meshComp = view.get<PrimitiveComponent>(entity);
            if (meshComp.meshId.empty() || meshComp.renderIndex == 0xFFFFFFFF) continue;

            if (meshComp.renderIndex < materialData.size()) {
                load(materialData[meshComp.renderIndex], meshComp);
                changed = true;
            }
        }
        return changed;
    }

    bool MaterialService::processDirtyMaterials() {
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

            if (meshComp.renderIndex < materialData.size()) {
                load(materialData[meshComp.renderIndex], meshComp);
                changed = true;
            } else {
                LOG_WARN("Dirty entity " + std::to_string(static_cast<uint32_t>(entity)) +
                         " has renderIndex out of bounds (" + std::to_string(meshComp.renderIndex) +
                         " >= " + std::to_string(materialData.size()) + ")");
            }
        }
        return changed;
    }

    void MaterialService::updateMeshMaterials() {
        bool changed = false;
        auto view = worldRepository->registry.view<PrimitiveComponent>();

        for (auto entity: view) {
            if (worldRepository->hiddenEntities.contains(entity)) continue;
            auto &meshComp = view.get<PrimitiveComponent>(entity);
            if (meshComp.meshId.empty()) continue;

            if (meshComp.renderIndex < materialData.size()) {
                load(materialData[meshComp.renderIndex], meshComp);
                changed = true;
            }
        }

        if (changed) {
            uploadMaterialData();
        }
    }

    void MaterialService::uploadMaterialData() {
        if (materialData.empty()) {
            LOG_TRACE("No material data to upload.");
            return;
        }

        LOG_DEBUG("Uploading " + std::to_string(materialData.size()) + " material data entries to GPU.");
        for (auto *frame: engineContext->registeredFrames) {
            auto *materialDataBuffer = frame->getResourceAs<BufferInstance>(RID_MATERIAL_DATA_BUFFER);
            if (materialDataBuffer != nullptr) {
                materialDataBuffer->update(materialData.data(), materialData.size() * sizeof(MaterialData));
            }
        }
    }

    void MaterialService::load(MaterialData &materialData, PrimitiveComponent &data) {
        materialData.transmission = data.transmissionFactor;
        materialData.thickness = data.thicknessFactor;
        materialData.ior = data.ior;
        materialData.isEmissive = worldRepository->hasComponent(data.getEntityId(), ComponentType::LIGHT) ? 1 : 0;

        materialData.albedoTextureId = 0;
        materialData.roughnessTextureId = 0;
        materialData.metallicTextureId = 0;

        if (!data.albedo.empty()) {
            auto *tex = textureService->stream(data.albedo);
            if (tex != nullptr) {
                materialData.albedoTextureId = textureService->getTextureIndex(data.albedo);
            }
        }
        if (!data.roughness.empty()) {
            auto *tex = textureService->stream(data.roughness);
            if (tex != nullptr) {
                materialData.roughnessTextureId = textureService->getTextureIndex(data.roughness);
            }
        }
        if (!data.metallic.empty()) {
            auto *tex = textureService->stream(data.metallic);
            if (tex != nullptr) {
                materialData.metallicTextureId = textureService->getTextureIndex(data.metallic);
            }
        }
    }
} // Metal
