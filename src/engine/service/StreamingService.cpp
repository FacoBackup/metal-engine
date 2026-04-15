#include "StreamingService.h"

#include "../EngineContext.h"
#include "../repository/WorldRepository.h"
#include "MeshService.h"
#include "TextureService.h"
#include "../../ApplicationEventContext.h"
#include "../dto/ResourceDisposalPayload.h"

#include "../resource/MeshInstance.h"
#include "../resource/TextureInstance.h"
#include "../../common/LoggerUtil.h"

#include "../dto/StaticGeometryComponent.h"
#include "../dto/InstancedGeometryComponent.h"
#include "../dto/AnimatedGeometryComponent.h"
#include "../dto/Material.h"
#include <nlohmann/json.hpp>
#include <fstream>

namespace Metal {
    static constexpr int MAX_TIMEOUT = 10000;

    void updateMaterialTexturesLastUse(const std::string& materialId, std::unordered_map<std::string, long long>& lastUse, EngineContext* engineContext) {
        if (materialId.empty()) return;
        std::ifstream is(materialId);
        if (is.is_open()) {
            try {
                nlohmann::json j;
                is >> j;
                Material mat;
                mat.fromJson(j);
                if (!mat.albedo.empty()) lastUse[mat.albedo] = engineContext->currentTimeMs;
                if (!mat.normal.empty()) lastUse[mat.normal] = engineContext->currentTimeMs;
                if (!mat.roughness.empty()) lastUse[mat.roughness] = engineContext->currentTimeMs;
                if (!mat.metallic.empty()) lastUse[mat.metallic] = engineContext->currentTimeMs;
                if (!mat.emissive.empty()) lastUse[mat.emissive] = engineContext->currentTimeMs;
            } catch (...) {}
        }
    }

    template<typename T>
    void disposeResources(AbstractResourceService<T> *service, std::unordered_map<std::string, long long> &lastUse, EngineContext *engineContext) {
        auto &resources = service->getResources();
        for (auto it = resources.begin(); it != resources.end();) {
            if (lastUse.contains(it->second->getId()) && !it->second->isNoDisposal() && (
                    engineContext->currentTimeMs - lastUse.at(it->second->getId())) >= MAX_TIMEOUT) {
                LOG_DEBUG(
                    "Disposing of " + it->first + " Since last use: " + std::to_string(engineContext->currentTimeMs -
                        lastUse.at(it->second->getId())));
                std::string id = it->first;
                ++it;
                ApplicationEventContext::dispatch("RESOURCE_DISPOSAL", std::make_shared<ResourceDisposalPayload>(id));
            } else {
                ++it;
            }
        }
    }

    void StreamingService::onSync() {
        auto view = worldRepository->registry.view<StaticGeometryComponent>();
        for (auto entity: view) {
            auto &meshComp = view.get<StaticGeometryComponent>(entity);
            if (!meshComp.meshId.empty()) {
                lastUse[meshComp.meshId] = engineContext->currentTimeMs;
            }
            if (!meshComp.materialId.empty()) {
                lastUse[meshComp.materialId] = engineContext->currentTimeMs;
                updateMaterialTexturesLastUse(meshComp.materialId, lastUse, engineContext);
            }
        }
        auto instancedView = worldRepository->registry.view<InstancedGeometryComponent>();
        for (auto entity: instancedView) {
            auto &meshComp = instancedView.get<InstancedGeometryComponent>(entity);
            if (!meshComp.meshId.empty()) {
                lastUse[meshComp.meshId] = engineContext->currentTimeMs;
            }
            if (!meshComp.materialId.empty()) {
                lastUse[meshComp.materialId] = engineContext->currentTimeMs;
                updateMaterialTexturesLastUse(meshComp.materialId, lastUse, engineContext);
            }
        }
        auto animatedView = worldRepository->registry.view<AnimatedGeometryComponent>();
        for (auto entity: animatedView) {
            auto &meshComp = animatedView.get<AnimatedGeometryComponent>(entity);
            if (!meshComp.meshId.empty()) {
                lastUse[meshComp.meshId] = engineContext->currentTimeMs;
            }
            if (!meshComp.materialId.empty()) {
                lastUse[meshComp.materialId] = engineContext->currentTimeMs;
                updateMaterialTexturesLastUse(meshComp.materialId, lastUse, engineContext);
            }
        }

        if ((engineContext->currentTime - sinceLastCleanup).count() >= MAX_TIMEOUT) {
            sinceLastCleanup = engineContext->currentTime;
            disposeResources(meshService, lastUse, engineContext);
            disposeResources(textureService, lastUse, engineContext);
        }
    }
}
