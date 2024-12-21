#include "EngineContext.h"

#include "../context/ApplicationContext.h"
#include "../context/runtime/BufferInstance.h"
#include "../context/runtime/RenderPass.h"
#include "render-pass/impl/OpaqueRenderPass.h"
#include "render-pass/impl/PostProcessingPass.h"
#include "render-pass/tools/GridRenderPass.h"
#include "service/camera/Camera.h"

namespace Metal {
    EngineContext::EngineContext(ApplicationContext &context) : AbstractRuntimeComponent(context) {
        fullScreenRenderPasses.push_back(std::make_unique<GridRenderPass>(context));
        postProcessingPasses.push_back(std::make_unique<PostProcessingPass>(context));
        gBufferPasses.push_back(std::make_unique<OpaqueRenderPass>(context));
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

        auto &camera = worldRepository.camera;
        if (globalDataNeedsUpdate) {
            globalDataNeedsUpdate = false;
            globalDataUBO.proj = camera.projectionMatrix;
            globalDataUBO.view = camera.viewMatrix;
            globalDataUBO.projView = camera.projViewMatrix;
            globalDataUBO.invProj = camera.invProjectionMatrix;
            globalDataUBO.invView = camera.invViewMatrix;
            globalDataUBO.cameraWorldPosition = camera.position;
            context.vulkanContext.coreBuffers.globalData->update(&globalDataUBO);
        }
        postProcessingUBO.distortionIntensity = camera.distortionIntensity;
        postProcessingUBO.chromaticAberrationIntensity = camera.chromaticAberrationIntensity;
        postProcessingUBO.distortionEnabled = camera.distortionEnabled;
        postProcessingUBO.chromaticAberrationEnabled = camera.chromaticAberrationEnabled;
        postProcessingUBO.vignetteEnabled = camera.vignetteEnabled;
        postProcessingUBO.vignetteStrength = camera.vignetteStrength;
        context.vulkanContext.coreBuffers.postProcessingSettings->update(&postProcessingUBO);

        context.vulkanContext.coreRenderPasses.gBufferPass->recordCommands(gBufferPasses);
        context.vulkanContext.coreRenderPasses.fullScreenPass->recordCommands(fullScreenRenderPasses);
        context.vulkanContext.coreRenderPasses.postProcessingPass->recordCommands(postProcessingPasses);
    }
}
