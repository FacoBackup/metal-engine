#include "EngineContext.h"

#include "../../enum/EngineResourceIDs.h"
#include "../../context/ApplicationContext.h"
#include "../../service/buffer/BufferInstance.h"
#include "../../service/descriptor/DescriptorBinding.h"
#include "../../service/voxel/SVOInstance.h"
#include "../../service/camera/Camera.h"
#include "../../service/framebuffer/FrameBufferInstance.h"
#include "../../service/texture/TextureInstance.h"
#include "../../repository/world/components/AtmosphereComponent.h"
#include "../../repository/world/impl/EntityComponent.h"
#include "../../repository/world/components/TransformComponent.h"

namespace Metal {
    void EngineContext::resetPathTracerAccumulationCount() const {
        CTX.engineRepository.pathTracerAccumulationCount = 0;
    }

    void EngineContext::onInitialize() {
    }

    void EngineContext::updateCurrentTime() {
        currentTime = Clock::now();
        std::chrono::duration<float> delta = currentTime - previousTime;
        deltaTime = delta.count();
        previousTime = currentTime;

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime.time_since_epoch());
        currentTimeMs = duration.count();

        if (start == -1) {
            start = currentTimeMs;
        }
    }

    void EngineContext::dispose() {
        if (currentFrame != nullptr) {
            currentFrame->dispose();
        }
    }

    void EngineContext::onSync() {
        updateCurrentTime();

        CTX.transformService.onSync();
        CTX.streamingService.onSync();
        CTX.cameraService.onSync();

        for (auto *frame: registeredFrames) {
            if (frame->getShouldRender()) {
                currentFrame = frame;

                if (updateLights || isFirstFrame) {
                    CTX.lightService.onSync();
                }

                isFirstFrame = false;

                updateGlobalData();
                currentFrame->onSync();

                frame->setShouldRender(false);
            }
        }

        CTX.rayTracingService.onSync();

        setUpdateLights(false);
        setCameraUpdated(false);
        setGISettingsUpdated(false);
    }

    void EngineContext::updateGlobalData() {
        auto &camera = CTX.worldRepository.camera;
        auto *fbo = currentFrame->getResourceAs<FrameBufferInstance>(  RID_POST_PROCESSING_FBO);
        globalDataUBO.previousProjView = globalDataUBO.projView;
        globalDataUBO.viewMatrix = camera.viewMatrix;
        globalDataUBO.projectionMatrix = camera.projectionMatrix;
        globalDataUBO.projView = camera.projViewMatrix;
        globalDataUBO.invProj = camera.invProjectionMatrix;
        globalDataUBO.invView = camera.invViewMatrix;
        globalDataUBO.cameraWorldPosition = camera.position;
        globalDataUBO.lightsCount = CTX.lightService.getCount();
        globalDataUBO.debugFlag = ShadingModes::IndexOfValue(CTX.editorRepository.shadingMode);
        CTX.engineRepository.pathTracerAccumulationCount++;
        globalDataUBO.pathTracerAccumulationCount = CTX.engineRepository.pathTracerAccumulationCount;
        globalDataUBO.globalFrameCount++;
        globalDataUBO.outputRes = {fbo->bufferWidth, fbo->bufferHeight};
        globalDataUBO.pathTracerMaxSamples = CTX.engineRepository.pathTracerMaxSamples;
        globalDataUBO.denoiserEnabled = CTX.engineRepository.denoiserEnabled && (globalDataUBO.debugFlag == LIT || globalDataUBO.debugFlag == LIGHTING_ONLY)? 1 : 0;

        entt::registry &reg = CTX.worldRepository.registry;
        auto view = reg.view<AtmosphereComponent>();
        if (auto it = view.begin(); it != view.end()) {
            auto &atmo = reg.get<AtmosphereComponent>(*it);
            atmosphereUBO.volumeScale = atmo.volumeScale;
            atmosphereUBO.albedo = atmo.albedo;
            atmosphereUBO.density = atmo.density;
            atmosphereUBO.g = atmo.g;
            atmosphereUBO.isAtmosphereEnabled = atmo.atmosphereEnabled ? 1 : 0;
            atmosphereUBO.isVolumeEnabled = atmo.volumeEnabled ? 1 : 0;
            atmosphereUBO.volumeShadowSteps = atmo.volumeShadowSteps;
            atmosphereUBO.scatteringAlbedo = atmo.scatteringAlbedo;
            atmosphereUBO.samples = atmo.samples;
            atmosphereUBO.sunPosition = atmo.sunPosition;
        }

        currentFrame->getResourceAs<BufferInstance>(RID_GLOBAL_DATA)->update(&globalDataUBO);
        currentFrame->getResourceAs<BufferInstance>(RID_ATMOSPHERE_DATA)->update(&atmosphereUBO);
    }
}
