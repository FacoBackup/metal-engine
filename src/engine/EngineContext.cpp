#include "EngineContext.h"

#include "../ApplicationContext.h"
#include "service/TransformService.h"
#include "service/StreamingService.h"
#include "service/RayTracingService.h"
#include "service/CameraService.h"
#include "service/LightService.h"
#include "service/VolumeService.h"
#include "repository/WorldRepository.h"
#include "repository/EngineRepository.h"
#include "../editor/repository/EditorRepository.h"

#include "../editor/enum/EngineResourceIDs.h"
#include "resource/BufferInstance.h"
#include "dto/TransformComponent.h"

namespace Metal {
    void EngineContext::resetPathTracerAccumulationCount() const {
        engineRepository->pathTracerAccumulationCount = 0;
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

    EngineFrameBuilder EngineContext::createFrame(const std::string &id) {
        EngineFrameBuilder frameBuilder(id);
        ctx->injectDependencies(&frameBuilder);
        return frameBuilder;
    }

    void EngineContext::setCurrentFrame(const std::string &id) {
        for (auto *frame: registeredFrames) {
            if (frame->getId() == id) {
                currentFrame = frame;
                currentFrame->setShouldRender(true);
                return;
            }
        }
        throw std::runtime_error("Frame not found");
    }

    void EngineContext::dispose() {
    }

    void EngineContext::onSync() {
        updateCurrentTime();

        transformService->onSync();
        streamingService->onSync();
        rayTracingService->onSync();
        cameraService->onSync();
        lightService->onSync();
        volumeService->onSync();

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
        auto &camera = worldRepository->camera;
        globalDataUBO.previousProjView = globalDataUBO.projView;
        globalDataUBO.viewMatrix = camera.viewMatrix;
        globalDataUBO.projectionMatrix = camera.projectionMatrix;
        globalDataUBO.projView = camera.projViewMatrix;
        globalDataUBO.invProj = camera.invProjectionMatrix;
        globalDataUBO.invView = camera.invViewMatrix;
        globalDataUBO.cameraWorldPosition = camera.position;
        globalDataUBO.volumeCount = volumeService->getCount();
        globalDataUBO.lightsCount = lightService->getCount();
        globalDataUBO.debugFlag = ShadingModes::IndexOfValue(editorRepository->shadingMode);
        engineRepository->pathTracerAccumulationCount++;
        globalDataUBO.pathTracerAccumulationCount = engineRepository->pathTracerAccumulationCount;
        globalDataUBO.globalFrameCount++;
        globalDataUBO.pathTracerMaxSamples = engineRepository->pathTracerMaxSamples;
        globalDataUBO.denoiserEnabled = engineRepository->denoiserEnabled && (
                                            globalDataUBO.debugFlag == LIT || globalDataUBO.debugFlag == LIGHTING_ONLY)
                                            ? 1
                                            : 0;

        lightService->computeSunInfo();
        globalDataUBO.sunPosition = lightService->getSunPosition();
        globalDataUBO.sunColor = lightService->getSunColor();
        currentFrame->getResourceAs<BufferInstance>(RID_GLOBAL_DATA)->update(&globalDataUBO);
    }
}
