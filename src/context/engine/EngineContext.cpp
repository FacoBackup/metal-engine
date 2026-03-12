#include "EngineContext.h"

#include "../../enum/EngineResourceIDs.h"
#include "../../context/ApplicationContext.h"
#include "../../service/buffer/BufferInstance.h"
#include "../../service/camera/Camera.h"
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
        CTX.rayTracingService.onSync();
        CTX.cameraService.onSync();
        CTX.lightService.onSync();
        CTX.volumeService.onSync();

        for (auto *frame: registeredFrames) {
            if (frame->getShouldRender()) {
                currentFrame = frame;
                updateGlobalData();
                currentFrame->onSync();

                frame->setShouldRender(false);
            }
        }

        setCameraUpdated(false);
        setGISettingsUpdated(false);
    }

    void EngineContext::updateGlobalData() {
        auto &camera = CTX.worldRepository.camera;
        globalDataUBO.previousProjView = globalDataUBO.projView;
        globalDataUBO.viewMatrix = camera.viewMatrix;
        globalDataUBO.projectionMatrix = camera.projectionMatrix;
        globalDataUBO.projView = camera.projViewMatrix;
        globalDataUBO.invProj = camera.invProjectionMatrix;
        globalDataUBO.invView = camera.invViewMatrix;
        globalDataUBO.cameraWorldPosition = camera.position;
        globalDataUBO.volumeCount = CTX.volumeService.getCount();
        globalDataUBO.lightsCount = CTX.lightService.getCount();
        globalDataUBO.debugFlag = ShadingModes::IndexOfValue(CTX.editorRepository.shadingMode);
        CTX.engineRepository.pathTracerAccumulationCount++;
        globalDataUBO.pathTracerAccumulationCount = CTX.engineRepository.pathTracerAccumulationCount;
        globalDataUBO.globalFrameCount++;
        globalDataUBO.pathTracerMaxSamples = CTX.engineRepository.pathTracerMaxSamples;
        globalDataUBO.denoiserEnabled = CTX.engineRepository.denoiserEnabled && (
                                            globalDataUBO.debugFlag == LIT || globalDataUBO.debugFlag == LIGHTING_ONLY)
                                            ? 1
                                            : 0;

        CTX.lightService.computeSunInfo();
        globalDataUBO.sunPosition = CTX.lightService.getSunPosition();
        globalDataUBO.sunColor = CTX.lightService.getSunColor();
        currentFrame->getResourceAs<BufferInstance>(RID_GLOBAL_DATA)->update(&globalDataUBO);
    }
}
