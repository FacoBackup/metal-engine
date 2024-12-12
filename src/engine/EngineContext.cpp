#include "EngineContext.h"

#include "../context/ApplicationContext.h"
#include "../context/runtime/BufferInstance.h"

namespace Metal {
    void EngineContext::dispose() const {
        for (auto &resource: resourceRepository.resources) {
            resource.second->dispose(context.getVulkanContext());
            delete resource.second;
        }
    }

    void EngineContext::onInitialize() {
        cameraSystem.onInitialize();
    }

    void EngineContext::onSync() {
        currentTime = Clock::now();
        deltaTime = static_cast<float>((currentTime - previousTime).count()) / 1000.f;
        if (start == -1) {
            start = Clock::now().time_since_epoch().count();
        }

        cameraSystem.onSync();
        if (globalDataNeedsUpdate) {
            globalDataNeedsUpdate = false;
            globalDataUBO.proj = cameraRepository.projectionMatrix;
            globalDataUBO.view = cameraRepository.viewMatrix;
            globalDataUBO.viewProj = cameraRepository.viewProjectionMatrix;
            globalDataUBO.invProj = cameraRepository.invProjectionMatrix;
            globalDataUBO.invView = cameraRepository.invViewMatrix;
            globalDataUBO.color = {1, 1, 0, 1};
            context.getVulkanContext().coreBuffers.globalData->update(&globalDataUBO);
        }
        renderPassSystem.onSync();
    }
}
