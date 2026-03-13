#include "StreamingService.h"

#include "../../ApplicationContext.h"
#include "../resource/SVOInstance.h"
#include "../resource/MeshInstance.h"
#include "../resource/TextureInstance.h"

#include "../dto/PrimitiveComponent.h"

namespace Metal {
    static constexpr int MAX_TIMEOUT = 10000;

    template<typename T>
    void disposeResources(AbstractResourceService<T> &service, std::unordered_map<std::string, long long> &lastUse) {
        auto &resources = service.getResources();
        for (auto it = resources.begin(); it != resources.end();) {
            if (lastUse.contains(it->second->getId()) && !it->second->isNoDisposal() && (
                    CTX.engineContext.currentTimeMs - lastUse.at(it->second->getId())) >= MAX_TIMEOUT) {
                LOG_DEBUG(
                    "Disposing of " + it->first + " Since last use: " + std::to_string(CTX.engineContext.currentTimeMs -
                        lastUse.at(it->second->getId())));
                std::string id = it->first;
                ++it;
                service.dispose(id);
            } else {
                ++it;
            }
        }
    }

    void StreamingService::onSync() {
        auto view = CTX.worldRepository.registry.view<PrimitiveComponent>();
        for (auto entity: view) {
            auto &meshComp = view.get<PrimitiveComponent>(entity);
            if (!meshComp.meshId.empty()) {
                lastUse[meshComp.meshId] = CTX.engineContext.currentTimeMs;
            }
            if (!meshComp.albedo.empty()) {
                lastUse[meshComp.albedo] = CTX.engineContext.currentTimeMs;
            }
            if (!meshComp.roughness.empty()) {
                lastUse[meshComp.roughness] = CTX.engineContext.currentTimeMs;
            }
            if (!meshComp.metallic.empty()) {
                lastUse[meshComp.metallic] = CTX.engineContext.currentTimeMs;
            }
        }

        if ((CTX.engineContext.currentTime - sinceLastCleanup).count() >= MAX_TIMEOUT) {
            sinceLastCleanup = CTX.engineContext.currentTime;
            disposeResources(CTX.meshService, lastUse);
            disposeResources(CTX.textureService, lastUse);
            disposeResources(CTX.voxelService, lastUse);
        }
    }
}
