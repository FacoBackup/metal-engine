#include "EngineContext.h"

#include "../ApplicationContext.h"
#include "service/TransformService.h"
#include "service/StreamingService.h"
#include "service/BLASService.h"
#include "service/TLASService.h"
#include "service/CameraService.h"
#include "service/LightService.h"
#include "service/PhysicsService.h"
#include "service/QuadTreeService.h"
#include "service/DirtyStateService.h"
#include "enum/DirtyType.h"
#include "repository/WorldRepository.h"
#include "repository/CameraRepository.h"
#include "repository/EngineRepository.h"
#include "editor/repository/EditorRepository.h"
#include "core/VulkanContext.h"

#include "editor/enum/EngineResourceIDs.h"
#include "resource/BufferInstance.h"
#include "dto/TransformComponent.h"
#include "service/MaterialService.h"

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

    void EngineContext::onSync() {
        updateCurrentTime();
        updateGlobalData();

        transformService->onSync();
        streamingService->onSync();
        /*if (vulkanContext->isRayTracingSupported()) {
            tlasService->onSync();
        }*/
        materialService->onSync();
        cameraService->onSync();
        dirtyStateService->consumeDirtyFlags(DirtyType::Camera);
        lightService->onSync();
        for (auto *frame: registeredFrames) {
            if (frame->getShouldRender()) {
                currentFrame = frame;
                currentFrame->onSync();

                frame->setShouldRender(false);
            }
        }

        setCameraUpdated(false);
    }

    void EngineContext::updateGlobalData() {
        globalDataUBO.previousProjView = globalDataUBO.projView;
        globalDataUBO.projView = cameraRepository->projViewMatrix;
        globalDataUBO.invProj = cameraRepository->invProjectionMatrix;
        globalDataUBO.invView = cameraRepository->invViewMatrix;
        globalDataUBO.invProjView = cameraRepository->invProjView;
        globalDataUBO.cameraWorldPosition = cameraRepository->position;
        globalDataUBO.lightsCount = lightService->getCount();
        globalDataUBO.debugFlag = ShadingModes::IndexOfValue(editorRepository->shadingMode);

        lightService->computeSunInfo();
        globalDataUBO.sunPosition = lightService->getSunPosition();

        engineRepository->pathTracerAccumulationCount++;
        globalDataUBO.pathTracerAccumulationCount = engineRepository->pathTracerAccumulationCount;
        globalDataUBO.globalFrameCount++;
        globalDataUBO.pathTracerMaxSamples = engineRepository->pathTracerMaxSamples;
        globalDataUBO.denoiserEnabled = engineRepository->denoiserEnabled && (
                                            globalDataUBO.debugFlag == LIT || globalDataUBO.debugFlag == GRID);
        globalDataUBO.isOrthographic = cameraRepository->isOrthographic;

        currentFrame->getResourceAs<BufferInstance>(RID_GLOBAL_DATA)->update(&globalDataUBO);
    }
}
