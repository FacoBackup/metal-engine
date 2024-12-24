#include "EngineContext.h"

#include "../../context/ApplicationContext.h"
#include "../../service/buffer/BufferInstance.h"
#include "CommandBufferRecorder.h"
#include "render-pass/impl/GBufferShadingPass.h"
#include "render-pass/impl/OpaqueRenderPass.h"
#include "render-pass/impl/PostProcessingPass.h"
#include "render-pass/tools/GridRenderPass.h"
#include "../../service/camera/Camera.h"

namespace Metal {
    EngineContext::EngineContext(ApplicationContext &context) : AbstractRuntimeComponent(context) {
        fullScreenRenderPasses.push_back(std::make_unique<GridRenderPass>(context));
        fullScreenRenderPasses.push_back(std::make_unique<GBufferShadingPass>(context));
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
        context.streamingRepository.onSync();
        context.cameraService.onSync();

        auto &camera = context.worldRepository.camera;
        updateGlobalData();
        context.coreBuffers.globalData->update(&globalDataUBO);
        postProcessingUBO.distortionIntensity = camera.distortionIntensity;
        postProcessingUBO.chromaticAberrationIntensity = camera.chromaticAberrationIntensity;
        postProcessingUBO.distortionEnabled = camera.distortionEnabled;
        postProcessingUBO.chromaticAberrationEnabled = camera.chromaticAberrationEnabled;
        postProcessingUBO.vignetteEnabled = camera.vignetteEnabled;
        postProcessingUBO.vignetteStrength = camera.vignetteStrength;
        context.coreBuffers.postProcessingSettings->update(&postProcessingUBO);

        context.coreRenderPasses.gBufferPass->recordCommands(gBufferPasses);
        context.coreRenderPasses.fullScreenPass->recordCommands(fullScreenRenderPasses);
        context.coreRenderPasses.postProcessingPass->recordCommands(postProcessingPasses);
    }

    void EngineContext::updateGlobalData() {
        auto &camera = context.worldRepository.camera;
        globalDataUBO.proj = camera.projectionMatrix;
        globalDataUBO.view = camera.viewMatrix;
        globalDataUBO.projView = camera.projViewMatrix;
        globalDataUBO.invProj = camera.invProjectionMatrix;
        globalDataUBO.invView = camera.invViewMatrix;
        globalDataUBO.cameraWorldPosition = camera.position;
        globalDataUBO.logDepthFC = 2.0f / (std::log(camera.projectionMatrix[0][0] + 1) / std::log(2));

        if (context.atmosphereRepository.incrementTime) {
            context.atmosphereRepository.elapsedTime += .0005f * context.atmosphereRepository.elapsedTimeSpeed;
        }
        globalDataUBO.sunPosition = glm::vec3(std::sin(context.atmosphereRepository.elapsedTime),
                                              std::cos(context.atmosphereRepository.elapsedTime),
                                              0) * context.atmosphereRepository.sunDistance;
        globalDataUBO.sunColor = CalculateSunColor(
            globalDataUBO.sunPosition.y / context.atmosphereRepository.sunDistance,
            context.atmosphereRepository.nightColor, context.atmosphereRepository.dawnColor,
            context.atmosphereRepository.middayColor);
    }


    glm::vec3 EngineContext::CalculateSunColor(const float elevation, glm::vec3 &nightColor, glm::vec3 &dawnColor,
                                               glm::vec3 &middayColor) {
        if (elevation <= -0.1f) {
            return nightColor;
        }
        if (elevation <= 0.0f) {
            float t = (elevation + 0.1f) / 0.1f;
            return BlendColors(nightColor, dawnColor, t);
        }
        if (elevation <= 0.5f) {
            float t = elevation / 0.5f;
            return BlendColors(dawnColor, middayColor, t);
        }
        return middayColor;
    }

    glm::vec3 EngineContext::BlendColors(glm::vec3 &c1, glm::vec3 &c2, float t) {
        return {
            (c1.x * (1 - t) + c2.x * t),
            (c1.y * (1 - t) + c2.y * t),
            (c1.z * (1 - t) + c2.z * t)
        };
    }
}
