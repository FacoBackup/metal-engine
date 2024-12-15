#include "EngineContext.h"

#include "../context/ApplicationContext.h"
#include "../context/runtime/BufferInstance.h"
#include "../context/runtime/RenderPass.h"
#include "render-pass/impl/OpaqueRenderPass.h"
#include "render-pass/tools/GridRenderPass.h"

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

        context.getVulkanContext().coreRenderPasses.fullScreenPass->recordCommands(fullScreenRenderPasses);
    }
}
