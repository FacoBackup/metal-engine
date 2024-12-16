#include "StreamingRepository.h"

#include "../../../context/ApplicationContext.h"
#include "../../../context/runtime/RuntimeResource.h"
#include "../../../context/runtime/MeshInstance.h"
#include <iostream>

#define MAX_TIMEOUT 1000
#define MAX_TRIES 5
#define DISPOSAL(T, S)\
for (auto &resource: S.getResources()) {\
    if (resource.second->lastUse.time_since_epoch().count() >= MAX_TIMEOUT) {\
         S.dispose(resource.second);\
    }\
}

#define STREAM(T, V)\
if (T.getResources().contains(id)) {\
    return static_cast<V*>(T.getResources().at(id));\
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
        STREAM(context.getVulkanContext().meshService, MeshInstance)
    }

    void StreamingRepository::onSync() {
        if ((context.getEngineContext().currentTime - sinceLastCleanup).count() >= MAX_TIMEOUT) {
            sinceLastCleanup = context.getEngineContext().currentTime;
            DISPOSAL(MeshInstance, context.getVulkanContext().meshService)
        }
    }
}
