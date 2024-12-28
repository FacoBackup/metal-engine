#include "StreamingRepository.h"

#include "../../context/ApplicationContext.h"
#include "../../service/voxel/SVOInstance.h"
#include "../../enum/LevelOfDetail.h"
#include "../../service/mesh/MeshInstance.h"
#include "../../service/texture/TextureInstance.h"
#include <iostream>
#include "../../repository/abstract/RuntimeResource.h"

#define MAX_TIMEOUT 1000
#define MAX_TRIES 5
#define DISPOSAL(R)\
for (auto it = R.begin(); it != R.end();) {\
    if (!it->second->isNoDisposal() && (it->second->lastUse - context.engineContext.currentTimeMs) >= MAX_TIMEOUT) {\
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
if (!id.empty() && T.getResources().contains(id + lod.suffix)) {\
    auto *e = T.getResources().at(id + lod.suffix);\
    e->lastUse = context.engineContext.currentTimeMs;\
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
        STREAM(context.meshService, MeshInstance)
    }

    SVOInstance *StreamingRepository::streamSVO(const std::string &id, const LevelOfDetail &lod) {
        STREAM(context.svoService, SVOInstance)
    }

    TextureInstance *StreamingRepository::streamTexture(const std::string &id, const LevelOfDetail &lod) {
        STREAM(context.textureService, TextureInstance)
    }

    void StreamingRepository::onSync() {
        if ((context.engineContext.currentTime - sinceLastCleanup).count() >= MAX_TIMEOUT) {
            sinceLastCleanup = context.engineContext.currentTime;
            DISPOSAL(context.meshService.getResources())
            DISPOSAL(context.textureService.getResources())
            DISPOSAL(context.svoService.getResources())
        }
    }
}
