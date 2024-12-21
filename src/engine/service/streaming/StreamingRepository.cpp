#include "StreamingRepository.h"

#include "../../../context/ApplicationContext.h"
#include "../../../context/runtime/RuntimeResource.h"
#include "../../../context/runtime/MeshInstance.h"
#include <iostream>

#include "../../../context/runtime/TextureInstance.h"

#define MAX_TIMEOUT 1000
#define MAX_TRIES 5
#define DISPOSAL(R)\
for (auto it = R.begin(); it != R.end();) {\
    if ((it->second->lastUse - context.engineContext.currentTimeMs) >= MAX_TIMEOUT) {\
        std::cout << "Disposing of " << it->first << " Since last use: " << (it->second->lastUse - context.engineContext.currentTimeMs) <<std::endl;\
        it->second->dispose(context.vulkanContext);\
        auto newIt = R.erase(it);\
        delete it->second;\
        it = newIt;\
    } else {\
        ++it;\
    }\
}

#define STREAM(T, V)\
if (T.getResources().contains(id)) {\
    auto *e = T.getResources().at(id);\
    e->lastUse = context.getEngineContext().currentTimeMs;\
    return dynamic_cast<V*>(e);\
}\
if (!tries.contains(id)) {\
    tries[id] = 0;\
}\
tries[id]++;\
if (tries[id] < MAX_TRIES) {\
    std::cout << "Steaming " << id << std::endl;\
    auto *instance = T.create(id, lod);\
    if (instance != nullptr) {\
        tries[id] = 0;\
    }\
    return instance;\
}\
return nullptr;

namespace Metal {
    MeshInstance *StreamingRepository::streamMesh(const std::string &id, const LevelOfDetail &lod) {
        STREAM(context.vulkanContext.meshService, MeshInstance)
    }

    TextureInstance *StreamingRepository::streamTexture(const std::string &id, const LevelOfDetail &lod) {
        STREAM(context.vulkanContext.textureService, TextureInstance)
    }

    void StreamingRepository::onSync() {
        if ((context.getEngineContext().currentTime - sinceLastCleanup).count() >= MAX_TIMEOUT) {
            sinceLastCleanup = context.getEngineContext().currentTime;
            DISPOSAL(context.vulkanContext.meshService.getResources())
            DISPOSAL(context.vulkanContext.textureService.getResources())
        }
    }
}
