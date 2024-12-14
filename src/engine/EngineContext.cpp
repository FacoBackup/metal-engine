#include "EngineContext.h"

#include "../context/ApplicationContext.h"
#include "../context/runtime/BufferInstance.h"

namespace Metal {
    void EngineContext::onInitialize() {
        cameraSystem.onInitialize();
    }

    void EngineContext::onSync() {
        currentTime = Clock::now();
        std::chrono::duration<float> delta = currentTime - previousTime;
        deltaTime = delta.count();
        previousTime = currentTime;
        if (start == -1) {
            start = Clock::now().time_since_epoch().count();
        }

        cameraSystem.onSync();
        if (globalDataNeedsUpdate) {
            globalDataNeedsUpdate = false;
            globalDataUBO.proj = cameraRepository.projectionMatrix;
            globalDataUBO.view = cameraRepository.viewMatrix;
            globalDataUBO.projView = cameraRepository.projViewMatrix;
            globalDataUBO.invProj = cameraRepository.invProjectionMatrix;
            globalDataUBO.invView = cameraRepository.invViewMatrix;
            globalDataUBO.cameraWorldPosition = cameraRepository.currentCamera->position;
            context.getVulkanContext().coreBuffers.globalData->update(&globalDataUBO);
        }
        renderPassSystem.onSync();
    }
}
