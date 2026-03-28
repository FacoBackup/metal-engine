#include "MaterialService.h"

#include "ApplicationEventContext.h"
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
#include "editor/dto/FieldModificationEvent.h"

namespace Metal {
    void MaterialService::onInitialize() {
        eventListener([this](const Event &event) {
            const auto payload = std::static_pointer_cast<InspectableEventPayload>(event.payload);
            if (const auto primitive = dynamic_cast<PrimitiveComponent *>(payload->reflectionInstance)) {
                dirtyStateService->markEntityDirty(primitive->getEntityId(), DirtyType::Material);
            }
        }, "PrimitiveComponent");
    }

    void MaterialService::onSync() {
        bool changed = processDirtyMaterials();

        if (changed) {
            uploadMaterialData();
        }
    }

    bool MaterialService::rebuildMaterialData() {
        LOG_INFO("Rebuilding material data...");
        fullRebuildNeeded = false;
        materialData.clear();
        materialCache.clear();

        auto view = worldRepository->registry.view<PrimitiveComponent>();

        bool changed = false;
        for (auto entity: view) {
            if (worldRepository->hiddenEntities.contains(entity)) continue;
            auto &meshComp = view.get<PrimitiveComponent>(entity);
            if (meshComp.meshId.empty()) continue;

            getMaterialIndex(meshComp);
            changed = true;
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
            if (meshComp.meshId.empty()) continue;

            std::string key = getMaterialKey(meshComp);
            if (materialCache.contains(key)) {
                uint32_t index = materialCache[key];
                load(materialData[index], meshComp);
                changed = true;
            } else {
                getMaterialIndex(meshComp);
                changed = true;
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

            getMaterialIndex(meshComp);
            changed = true;
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

    void MaterialService::clear() {
        materialData.clear();
        materialCache.clear();
        fullRebuildNeeded = false;
    }

    void MaterialService::load(MaterialData &dest, const PrimitiveComponent &data) {
        dest.transmission = data.transmissionFactor;
        dest.thickness = data.thicknessFactor;
        dest.ior = data.ior;
        dest.isEmissive = worldRepository->hasComponent(data.getEntityId(), LIGHT) ? 1 : 0;

        dest.albedoTextureId = 0;
        dest.roughnessTextureId = 0;
        dest.metallicTextureId = 0;

        if (!data.albedo.empty()) {
            auto *tex = textureService->stream(data.albedo);
            if (tex != nullptr) {
                dest.albedoTextureId = textureService->getTextureIndex(data.albedo);
            }
        }
        if (!data.roughness.empty()) {
            auto *tex = textureService->stream(data.roughness);
            if (tex != nullptr) {
                dest.roughnessTextureId = textureService->getTextureIndex(data.roughness);
            }
        }
        if (!data.metallic.empty()) {
            auto *tex = textureService->stream(data.metallic);
            if (tex != nullptr) {
                dest.metallicTextureId = textureService->getTextureIndex(data.metallic);
            }
        }
    }

    std::string MaterialService::getMaterialKey(const PrimitiveComponent &component) const {
        return component.albedo + "|" + component.roughness + "|" + component.metallic + "|" +
               std::to_string(component.transmissionFactor) + "|" +
               std::to_string(component.thicknessFactor) + "|" +
               std::to_string(component.ior) + "|" +
               std::to_string(worldRepository->hasComponent(component.getEntityId(), LIGHT));
    }

    uint32_t MaterialService::getMaterialIndex(const PrimitiveComponent &component) {
        std::string key = getMaterialKey(component);
        if (materialCache.contains(key)) {
            return materialCache[key];
        }

        uint32_t index = static_cast<uint32_t>(materialData.size());
        materialData.emplace_back();
        load(materialData.back(), component);
        materialCache[key] = index;

        return index;
    }
} // Metal
