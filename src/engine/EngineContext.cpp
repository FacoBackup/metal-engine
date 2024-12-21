#include "EngineContext.h"

#include "../context/ApplicationContext.h"
#include "../context/runtime/BufferInstance.h"
#include "../context/runtime/RenderPass.h"
#include "render-pass/impl/OpaqueRenderPass.h"
#include "render-pass/tools/GridRenderPass.h"
#include "service/camera/Camera.h"

namespace Metal {
    EngineContext::EngineContext(ApplicationContext &context) : AbstractRuntimeComponent(context) {
        fullScreenRenderPasses.push_back(std::make_unique<GridRenderPass>(context));
        fullScreenRenderPasses.push_back(std::make_unique<OpaqueRenderPass>(context));
    }

    void EngineContext::onSync() {
        currentTime = Clock::now();
        std::chrono::duration<float> delta = currentTime - previousTime;
        deltaTime = delta.count();
        previousTime = currentTime;

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime.time_since_epoch());
        currentTimeMs = duration.count();

        if (start == -1) {
            start = currentTimeMs;
        }
        streamingRepository.onSync();
        cameraService.onSync();
        if (globalDataNeedsUpdate) {
            globalDataNeedsUpdate = false;
            globalDataUBO.proj = worldRepository.camera.projectionMatrix;
            globalDataUBO.view = worldRepository.camera.viewMatrix;
            globalDataUBO.projView = worldRepository.camera.projViewMatrix;
            globalDataUBO.invProj = worldRepository.camera.invProjectionMatrix;
            globalDataUBO.invView = worldRepository.camera.invViewMatrix;
            globalDataUBO.cameraWorldPosition = worldRepository.camera.position;
            context.getVulkanContext().coreBuffers.globalData->update(&globalDataUBO);
        }

        context.getVulkanContext().coreRenderPasses.fullScreenPass->recordCommands(fullScreenRenderPasses);
    }
}
