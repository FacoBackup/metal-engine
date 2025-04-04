#include "StreamingRepository.h"

#include "../../context/ApplicationContext.h"
#include "../../enum/LevelOfDetail.h"
#include "../../service/texture/TextureInstance.h"
#include <iostream>

#include "../../dto/buffers/MaterialInfo.h"
#include "../../repository/abstract/RuntimeResource.h"

#define MAX_TIMEOUT 1000
#define MAX_TRIES 5
#define DISPOSAL(R)\
for (auto it = R.begin(); it != R.end();) {\
    if (lastUse.contains(it->second->getId()) && !it->second->isNoDisposal() && (lastUse.at(it->second->getId()) - context.engineContext.currentTimeMs) >= MAX_TIMEOUT) {\
        std::cout << "Disposing of " << it->first << " Since last use: " << (lastUse.at(it->second->getId()) - context.engineContext.currentTimeMs) <<std::endl;\
        it->second->dispose(context.vulkanContext);\
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
    lastUse.emplace(e->getId(), context.engineContext.currentTimeMs);\
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
                lastUse[dep] = context.engineContext.currentTimeMs;\
            }\
        }\
    }\
    return instance;\
}\
return nullptr;



#define STREAM_NO_LOD(T, V)\
if (!id.empty() && T.getResources().contains(id)) {\
auto *e = T.getResources().at(id);\
lastUse.emplace(e->getId(), context.engineContext.currentTimeMs);\
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
            lastUse[dep] = context.engineContext.currentTimeMs;\
        }\
    }\
}\
return instance;\
}\
return nullptr;

namespace Metal {
    MaterialInfo *StreamingRepository::streamMaterial(const std::string &id) {
        STREAM_NO_LOD(context.materialService, MaterialInfo)
    }

    TextureInstance *StreamingRepository::streamTexture(const std::string &id, const LevelOfDetail &lod) {
        STREAM(context.textureService, TextureInstance)
    }

    void StreamingRepository::onSync() {
        if ((context.engineContext.currentTime - sinceLastCleanup).count() >= MAX_TIMEOUT) {
            sinceLastCleanup = context.engineContext.currentTime;
            DISPOSAL(context.textureService.getResources())
            DISPOSAL(context.materialService.getResources())
        }
    }
}
