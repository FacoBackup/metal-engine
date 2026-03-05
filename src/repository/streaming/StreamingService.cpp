#include "StreamingService.h"

#include "../../context/ApplicationContext.h"
#include "../../service/voxel/SVOInstance.h"
#include "../../service/mesh/MeshInstance.h"
#include "../../service/texture/TextureInstance.h"

namespace Metal {
    static constexpr int MAX_TIMEOUT = 10000;
    static constexpr int MAX_TRIES = 5;

    template<typename T>
    T *stream(IStreamable<T> &service, const std::string &id,
                            std::unordered_map<std::string, long long> &lastUse,
                            std::unordered_map<std::string, unsigned int> &tries) {
        if (!id.empty() && service.getResources().contains(id)) {
            auto *e = service.getResource(id);
            lastUse[e->getId()] = CTX.engineContext.currentTimeMs;
            return e;
        }
        if (!tries.contains(id)) {
            tries[id] = 0;
        }
        tries[id]++;
        if (tries[id] < MAX_TRIES) {
            LOG_DEBUG("Streaming " + id);
            auto *instance = service.create(id);
            if (instance != nullptr) {
                tries[id] = 0;
                for (auto &dep: instance->getDependencies()) {
                    if (lastUse.contains(dep)) {
                        lastUse[dep] = CTX.engineContext.currentTimeMs;
                    }
                }
            }
            return instance;
        }
        return nullptr;
    }

    SVOInstance *StreamingService::streamSVO(const std::string &id) {
        return stream(CTX.voxelService, id, lastUse, tries);
    }

    MeshInstance *StreamingService::streamMesh(const std::string &id) {
        return stream(CTX.meshService, id, lastUse, tries);
    }

    TextureInstance *StreamingService::streamTexture(const std::string &id) {
        return stream(CTX.textureService, id, lastUse, tries);
    }

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
        if ((CTX.engineContext.currentTime - sinceLastCleanup).count() >= MAX_TIMEOUT) {
            sinceLastCleanup = CTX.engineContext.currentTime;
            disposeResources(CTX.meshService, lastUse);
            disposeResources(CTX.textureService, lastUse);
            disposeResources(CTX.voxelService, lastUse);
        }
    }
}
