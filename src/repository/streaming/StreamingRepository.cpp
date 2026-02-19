#include "StreamingRepository.h"

#include "../../context/ApplicationContext.h"
#include "../../service/voxel/SVOInstance.h"
#include "../../enum/LevelOfDetail.h"
#include "../../service/mesh/MeshInstance.h"
#include "../../service/texture/TextureInstance.h"
#include <iostream>
#include "../../repository/abstract/RuntimeResource.h"
#include "../../service/material/MaterialInstance.h"

#define MAX_TIMEOUT 1000
#define MAX_TRIES 5
#define DISPOSAL(R)\
for (auto it = R.begin(); it != R.end();) {\
    if (lastUse.contains(it->second->getId()) && !it->second->isNoDisposal() && (lastUse.at(it->second->getId()) - ApplicationContext::Get().engineContext.currentTimeMs) >= MAX_TIMEOUT) {\
        LOG_INFO("Disposing of " + it->first + " Since last use: " + std::to_string(lastUse.at(it->second->getId()) - ApplicationContext::Get().engineContext.currentTimeMs));\
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
    lastUse.emplace(e->getId(), ApplicationContext::Get().engineContext.currentTimeMs);\
    return dynamic_cast<V*>(e);\
}\
if (!tries.contains(id)) {\
    tries[id] = 0;\
}\
tries[id]++;\
if (tries[id] < MAX_TRIES) {\
    LOG_INFO("Streaming " + id);\
    auto *instance = T.create(id, lod);\
    if (instance != nullptr) {\
        tries[id] = 0;\
        for(auto &dep : instance->getDependencies()){\
            if (lastUse.contains(dep)) {\
                lastUse[dep] = ApplicationContext::Get().engineContext.currentTimeMs;\
            }\
        }\
    }\
    return instance;\
}\
return nullptr;



#define STREAM_NO_LOD(T, V)\
if (!id.empty() && T.getResources().contains(id)) {\
auto *e = T.getResources().at(id);\
lastUse.emplace(e->getId(), ApplicationContext::Get().engineContext.currentTimeMs);\
return dynamic_cast<V*>(e);\
}\
if (!tries.contains(id)) {\
tries[id] = 0;\
}\
tries[id]++;\
if (tries[id] < MAX_TRIES) {\
LOG_INFO("Streaming " + id);\
auto *instance = T.create(id);\
if (instance != nullptr) {\
    tries[id] = 0;\
    for(auto &dep : instance->getDependencies()){\
        if (lastUse.contains(dep)) {\
            lastUse[dep] = ApplicationContext::Get().engineContext.currentTimeMs;\
        }\
    }\
}\
return instance;\
}\
return nullptr;

namespace Metal {
    MaterialInstance *StreamingRepository::streamMaterial(const std::string &id) {
        STREAM_NO_LOD(ApplicationContext::Get().materialService, MaterialInstance)
    }

    SVOInstance *StreamingRepository::streamSVO(const std::string &id) {
        STREAM_NO_LOD(ApplicationContext::Get().voxelService, SVOInstance)
    }

    MeshInstance *StreamingRepository::streamMesh(const std::string &id, const LevelOfDetail &lod) {
        STREAM(ApplicationContext::Get().meshService, MeshInstance)
    }

    TextureInstance *StreamingRepository::streamTexture(const std::string &id, const LevelOfDetail &lod) {
        STREAM(ApplicationContext::Get().textureService, TextureInstance)
    }

    void StreamingRepository::onSync() {
        if ((ApplicationContext::Get().engineContext.currentTime - sinceLastCleanup).count() >= MAX_TIMEOUT) {
            sinceLastCleanup = ApplicationContext::Get().engineContext.currentTime;
            DISPOSAL(ApplicationContext::Get().meshService.getResources())
            DISPOSAL(ApplicationContext::Get().textureService.getResources())
            DISPOSAL(ApplicationContext::Get().voxelService.getResources())
            DISPOSAL(ApplicationContext::Get().materialService.getResources())
        }
    }
}
