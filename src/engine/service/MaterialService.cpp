#include "MaterialService.h"
#include "ApplicationEventContext.h"
#include "../../common/serialization-definitions.h"
#include "../../common/LoggerUtil.h"
#include "../dto/MaterialData.h"
#include "../dto/Material.h"
#include "TextureService.h"
#include "../repository/WorldRepository.h"
#include "../dto/StaticGeometryComponent.h"
#include "../dto/InstancedGeometryComponent.h"
#include "../dto/AnimatedGeometryComponent.h"
#include "../dto/TransformComponent.h"
#include "../EngineContext.h"
#include "BufferService.h"
#include "DirtyStateService.h"
#include "../resource/BufferInstance.h"
#include "../../editor/enum/EngineResourceIDs.h"
#include "editor/dto/FieldModificationEvent.h"
#include <fstream>
#include <cereal/archives/json.hpp>

namespace Metal {
    void MaterialService::onInitialize() {
        eventListener([this](const Event &event) {
            const auto payload = std::static_pointer_cast<InspectableEventPayload>(event.payload);
            if (const auto primitive = dynamic_cast<StaticGeometryComponent *>(payload->reflectionInstance)) {
                dirtyStateService->markEntityDirty(primitive->getEntityId(), DirtyType::Material);
            } else if (const auto instanced = dynamic_cast<InstancedGeometryComponent *>(payload->reflectionInstance)) {
                dirtyStateService->markEntityDirty(instanced->getEntityId(), DirtyType::Material);
            } else if (const auto animated = dynamic_cast<AnimatedGeometryComponent *>(payload->reflectionInstance)) {
                dirtyStateService->markEntityDirty(animated->getEntityId(), DirtyType::Material);
            }
        }, "StaticGeometryComponent");

        eventListener([this](const Event &event) {
            const auto payload = std::static_pointer_cast<InspectableEventPayload>(event.payload);
            if (const auto primitive = dynamic_cast<InstancedGeometryComponent *>(payload->reflectionInstance)) {
                dirtyStateService->markEntityDirty(primitive->getEntityId(), DirtyType::Material);
            }
        }, "InstancedGeometryComponent");

        eventListener([this](const Event &event) {
            const auto payload = std::static_pointer_cast<InspectableEventPayload>(event.payload);
            if (const auto primitive = dynamic_cast<AnimatedGeometryComponent *>(payload->reflectionInstance)) {
                dirtyStateService->markEntityDirty(primitive->getEntityId(), DirtyType::Material);
            }
        }, "AnimatedGeometryComponent");
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

        auto view = worldRepository->registry.view<StaticGeometryComponent>();

        bool changed = false;
        for (auto entity: view) {
            if (worldRepository->hiddenEntities.contains(entity)) continue;
            auto &meshComp = view.get<StaticGeometryComponent>(entity);
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

            if (worldRepository->registry.all_of<StaticGeometryComponent>(entity)) {
                auto &meshComp = worldRepository->registry.get<StaticGeometryComponent>(entity);
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
            } else if (worldRepository->registry.all_of<InstancedGeometryComponent>(entity)) {
                auto &meshComp = worldRepository->registry.get<InstancedGeometryComponent>(entity);
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
            } else if (worldRepository->registry.all_of<AnimatedGeometryComponent>(entity)) {
                auto &meshComp = worldRepository->registry.get<AnimatedGeometryComponent>(entity);
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
        }
        return changed;
    }

    void MaterialService::updateMeshMaterials() {
        bool changed = false;
        auto view = worldRepository->registry.view<StaticGeometryComponent>();

        for (auto entity: view) {
            if (worldRepository->hiddenEntities.contains(entity)) continue;
            auto &meshComp = view.get<StaticGeometryComponent>(entity);
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

    void MaterialService::load(MaterialData &dest, const StaticGeometryComponent &data) {
        dest.albedoTextureId = 0;
        dest.roughnessTextureId = 0;
        dest.metallicTextureId = 0;
        dest.normalTextureId = 0;
        dest.emissiveTextureId = 0;
        dest.roughnessFactor = 1.0f;
        dest.metallicFactor = 0.0f;
        dest.isEmissive = worldRepository->hasComponent(data.getEntityId(), LIGHT) ? 1 : 0;

        if (!data.materialId.empty()) {
            std::ifstream is(data.materialId);
            if (is.is_open()) {
                Material mat;
                try {
                    nlohmann::json j;
                    is >> j;
                    mat.fromJson(j);
                    dest.albedoTextureId = textureService->getTextureIndex(mat.albedo);
                    dest.roughnessTextureId = textureService->getTextureIndex(mat.roughness);
                    dest.metallicTextureId = textureService->getTextureIndex(mat.metallic);
                    dest.normalTextureId = textureService->getTextureIndex(mat.normal);
                    dest.emissiveTextureId = textureService->getTextureIndex(mat.emissive);
                    dest.roughnessFactor = mat.roughnessFactor;
                    dest.metallicFactor = mat.metallicFactor;
                    dest.parallaxHeightScale = mat.parallaxScale;
                    dest.parallaxLayers = mat.parallaxLayers;
                    if (!mat.emissive.empty()) dest.isEmissive = 1;
                } catch (...) {}
            }
        }
    }

    void MaterialService::load(MaterialData &dest, const InstancedGeometryComponent &data) {
        dest.isEmissive = worldRepository->hasComponent(data.getEntityId(), LIGHT) ? 1 : 0;
        dest.albedoTextureId = 0;
        dest.roughnessTextureId = 0;
        dest.metallicTextureId = 0;
        dest.normalTextureId = 0;
        dest.emissiveTextureId = 0;
        dest.roughnessFactor = 1.0f;
        dest.metallicFactor = 0.0f;
        dest.parallaxHeightScale = 0.05f;
        dest.parallaxLayers = 32;
    }

    void MaterialService::load(MaterialData &dest, const AnimatedGeometryComponent &data) {
        dest.isEmissive = worldRepository->hasComponent(data.getEntityId(), LIGHT) ? 1 : 0;
        dest.albedoTextureId = 0;
        dest.roughnessTextureId = 0;
        dest.metallicTextureId = 0;
        dest.normalTextureId = 0;
        dest.emissiveTextureId = 0;
        dest.roughnessFactor = 1.0f;
        dest.metallicFactor = 0.0f;
        dest.parallaxHeightScale = 0.05f;
        dest.parallaxLayers = 32;
    }

    std::string MaterialService::getMaterialKey(const StaticGeometryComponent &component) const {
        return component.materialId + "|" + std::to_string(worldRepository->hasComponent(component.getEntityId(), LIGHT));
    }

    std::string MaterialService::getMaterialKey(const InstancedGeometryComponent &component) const {
        return component.materialId + "|" + std::to_string(worldRepository->hasComponent(component.getEntityId(), LIGHT));
    }

    std::string MaterialService::getMaterialKey(const AnimatedGeometryComponent &component) const {
        return component.materialId + "|" + std::to_string(worldRepository->hasComponent(component.getEntityId(), LIGHT));
    }

    uint32_t MaterialService::getMaterialIndex(StaticGeometryComponent &component) {
        std::string key = getMaterialKey(component);
        if (materialCache.contains(key)) {
            return materialCache[key];
        }

        uint32_t index = static_cast<uint32_t>(materialData.size());
        materialData.emplace_back();
        load(materialData.back(), component);
        materialCache[key] = index;
        component.materialIndex = index;
        return index;
    }

    uint32_t MaterialService::getMaterialIndex(InstancedGeometryComponent &component) {
        std::string key = getMaterialKey(component);
        if (materialCache.contains(key)) {
            return materialCache[key];
        }

        uint32_t index = static_cast<uint32_t>(materialData.size());
        materialData.emplace_back();
        load(materialData.back(), component);
        materialCache[key] = index;
        component.materialIndex = index;

        return index;
    }

    uint32_t MaterialService::getMaterialIndex(AnimatedGeometryComponent &component) {
        std::string key = getMaterialKey(component);
        if (materialCache.contains(key)) {
            return materialCache[key];
        }

        uint32_t index = static_cast<uint32_t>(materialData.size());
        materialData.emplace_back();
        load(materialData.back(), component);
        materialCache[key] = index;
        component.materialIndex = index;

        return index;
    }
} // Metal
