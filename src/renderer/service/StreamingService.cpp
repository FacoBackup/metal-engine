#include "StreamingService.h"

#include "../../MetalContextProvider.h"
#include "../../common/enum/LevelOfDetail.h"
#include "../data/TextureInstance.h"
#include <iostream>

#include "../data/MaterialInfo.h"
#include "../../common/RuntimeResource.h"

#define MAX_TIMEOUT 1000
#define MAX_TRIES 5
#define DISPOSAL(R)\
for (auto it = R.begin(); it != R.end();) {\
    if (lastUse.contains(it->second->getId()) && !it->second->isNoDisposal() && (lastUse.at(it->second->getId()) - SINGLETONS.engineContext.currentTimeMs) >= MAX_TIMEOUT) {\
        std::cout << "Disposing of " << it->first << " Since last use: " << (lastUse.at(it->second->getId()) - SINGLETONS.engineContext.currentTimeMs) <<std::endl;\
        it->second->dispose();\
        auto newIt = R.erase(it);\
        delete it->second;\
        it = newIt;\
    } else {\
        ++it;\
    }\
}

#define STREAM(T, V)\
if (!id.empty() && T.getResources().contains(id + lod.suffix)) {\
    auto *e = T.getResources().at(id + lod.suffix);\
    lastUse.emplace(e->getId(), SINGLETONS.engineContext.currentTimeMs);\
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
        for(auto &dep : instance->getDependencies()){\
            if (lastUse.contains(dep)) {\
                lastUse[dep] = SINGLETONS.engineContext.currentTimeMs;\
            }\
        }\
    }\
    return instance;\
}\
return nullptr;



#define STREAM_NO_LOD(T, V)\
if (!id.empty() && T.getResources().contains(id)) {\
auto *e = T.getResources().at(id);\
lastUse.emplace(e->getId(), SINGLETONS.engineContext.currentTimeMs);\
return dynamic_cast<V*>(e);\
}\
if (!tries.contains(id)) {\
tries[id] = 0;\
}\
tries[id]++;\
if (tries[id] < MAX_TRIES) {\
std::cout << "Steaming " << id << std::endl;\
auto *instance = T.create(id);\
if (instance != nullptr) {\
    tries[id] = 0;\
    for(auto &dep : instance->getDependencies()){\
        if (lastUse.contains(dep)) {\
            lastUse[dep] = SINGLETONS.engineContext.currentTimeMs;\
        }\
    }\
}\
return instance;\
}\
return nullptr;

namespace Metal {
    MaterialInfo *StreamingService::streamMaterial(const std::string &id) {
        STREAM_NO_LOD(SINGLETONS.materialService, MaterialInfo)
    }

    TextureInstance *StreamingService::streamTexture(const std::string &id, const LevelOfDetail &lod) {
        STREAM(SINGLETONS.textureService, TextureInstance)
    }

    void StreamingService::onSync() {
        if ((SINGLETONS.engineContext.currentTime - sinceLastCleanup).count() >= MAX_TIMEOUT) {
            sinceLastCleanup = SINGLETONS.engineContext.currentTime;
            DISPOSAL(SINGLETONS.textureService.getResources())
            DISPOSAL(SINGLETONS.materialService.getResources())
        }
    }
}
