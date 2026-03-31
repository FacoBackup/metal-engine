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

namespace Metal {
    static constexpr int MAX_TIMEOUT = 10000;

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
            if (!meshComp.albedo.empty()) {
                lastUse[meshComp.albedo] = engineContext->currentTimeMs;
            }
            if (!meshComp.roughness.empty()) {
                lastUse[meshComp.roughness] = engineContext->currentTimeMs;
            }
            if (!meshComp.metallic.empty()) {
                lastUse[meshComp.metallic] = engineContext->currentTimeMs;
            }
        }
        auto instancedView = worldRepository->registry.view<InstancedGeometryComponent>();
        for (auto entity: instancedView) {
            auto &meshComp = instancedView.get<InstancedGeometryComponent>(entity);
            if (!meshComp.meshId.empty()) {
                lastUse[meshComp.meshId] = engineContext->currentTimeMs;
            }
            if (!meshComp.albedo.empty()) {
                lastUse[meshComp.albedo] = engineContext->currentTimeMs;
            }
            if (!meshComp.roughness.empty()) {
                lastUse[meshComp.roughness] = engineContext->currentTimeMs;
            }
            if (!meshComp.metallic.empty()) {
                lastUse[meshComp.metallic] = engineContext->currentTimeMs;
            }
        }
        auto animatedView = worldRepository->registry.view<AnimatedGeometryComponent>();
        for (auto entity: animatedView) {
            auto &meshComp = animatedView.get<AnimatedGeometryComponent>(entity);
            if (!meshComp.meshId.empty()) {
                lastUse[meshComp.meshId] = engineContext->currentTimeMs;
            }
            if (!meshComp.albedo.empty()) {
                lastUse[meshComp.albedo] = engineContext->currentTimeMs;
            }
            if (!meshComp.roughness.empty()) {
                lastUse[meshComp.roughness] = engineContext->currentTimeMs;
            }
            if (!meshComp.metallic.empty()) {
                lastUse[meshComp.metallic] = engineContext->currentTimeMs;
            }
        }

        if ((engineContext->currentTime - sinceLastCleanup).count() >= MAX_TIMEOUT) {
            sinceLastCleanup = engineContext->currentTime;
            disposeResources(meshService, lastUse, engineContext);
            disposeResources(textureService, lastUse, engineContext);
        }
    }
}
