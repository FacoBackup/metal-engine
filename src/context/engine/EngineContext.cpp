#include "EngineContext.h"

#include "../../enum/EngineResourceIDs.h"
#include "../../context/ApplicationContext.h"
#include "../../service/buffer/BufferInstance.h"
#include "../../service/descriptor/DescriptorBinding.h"
#include "../../service/voxel/SVOInstance.h"
#include "../../service/camera/Camera.h"
#include "../../service/framebuffer/FrameBufferInstance.h"
#include "../../service/texture/TextureInstance.h"

namespace Metal {
    void EngineContext::resetPathTracerAccumulationCount() const {
        CTX.engineRepository.pathTracerAccumulationCount = 0;
    }

    void EngineContext::onInitialize() {
        CTX.worldGridService.onSync();
    }

    void EngineContext::updateTileData() {
        if (CTX.worldGridRepository.hasMainTileChanged) {
            unsigned int i = 0;
            std::vector<DescriptorBinding> bindings{};
            for (auto *tile: CTX.worldGridRepository.getLoadedTiles()) {
                if (tile != nullptr) {
                    const auto *svo = CTX.streamingRepository.streamSVO(tile->id);
                    if (svo != nullptr) {
                        tileInfoUBO.tileCenterValid[i] = glm::vec4(tile->x, 0,
                                                                   tile->z, 1);
                        tileInfoUBO.voxelBufferOffset[i] = svo->voxelBufferOffset;
                        i++;
                    }
                }
            }

            for (unsigned int j = i; j < 9; j++) {
                tileInfoUBO.tileCenterValid[i].w = 0;
            }

            if (i > 0) {
                currentFrame->getResourceAs<BufferInstance>(RID_TILE_INFO)->update(tileInfoUBO.tileCenterValid.data());
            }
            CTX.worldGridRepository.hasMainTileChanged = false;
        }
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
        CTX.worldGridService.onSync();
        CTX.streamingRepository.onSync();
        CTX.cameraService.onSync();

        for (auto *frame: registeredFrames) {
            if (frame->getShouldRender()) {
                currentFrame = frame;

                updateTileData();
                if (updateLights) {
                    CTX.lightService.onSync();
                }

                if (updateVolumes) {
                    CTX.volumeService.onSync();
                }

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
        globalDataUBO.outputRes.x = fbo->bufferWidth;
        globalDataUBO.outputRes.y = fbo->bufferHeight;
        globalDataUBO.viewMatrix = camera.viewMatrix;
        globalDataUBO.projectionMatrix = camera.projectionMatrix;
        globalDataUBO.projView = camera.projViewMatrix;
        globalDataUBO.invProj = camera.invProjectionMatrix;
        globalDataUBO.invView = camera.invViewMatrix;
        globalDataUBO.cameraWorldPosition = camera.position;
        globalDataUBO.pathTracerMultiplier = CTX.engineRepository.pathTracerMultiplier;
        globalDataUBO.volumeCount = CTX.volumeService.getCount();
        globalDataUBO.lightsCount = CTX.lightService.getCount();
        globalDataUBO.volumeShadowSteps = CTX.engineRepository.volumeShadowSteps;
        globalDataUBO.isAtmosphereEnabled = CTX.engineRepository.atmosphereEnabled;

        globalDataUBO.enableSurfaceCache = CTX.engineRepository.enableSurfaceCache ? 1 : 0;
        globalDataUBO.multipleImportanceSampling = CTX.engineRepository.multipleImportanceSampling;
        globalDataUBO.pathTracerMaxSamples = CTX.engineRepository.pathTracerMaxSamples;
        globalDataUBO.pathTracerSamples = CTX.engineRepository.pathTracerSamples;
        globalDataUBO.pathTracerBounces = CTX.engineRepository.pathTracerBounces;
        globalDataUBO.giTileSubdivision = CTX.engineRepository.giTileSubdivision;
        globalDataUBO.giEmissiveFactor = CTX.engineRepository.giEmissiveFactor;

        globalDataUBO.debugFlag = ShadingModes::IndexOfValue(CTX.editorRepository.shadingMode);
        globalDataUBO.surfaceCacheWidth = SURFACE_CACHE_RES;
        globalDataUBO.surfaceCacheHeight = SURFACE_CACHE_RES;
        CTX.engineRepository.pathTracerAccumulationCount++;
        globalDataUBO.pathTracerAccumulationCount = CTX.engineRepository.pathTracerAccumulationCount;
        globalDataUBO.globalFrameCount++;

        if (CTX.engineRepository.incrementTime) {
            CTX.engineRepository.elapsedTime += .0005f * CTX.engineRepository.elapsedTimeSpeed;
            setGISettingsUpdated(true);
            updateLights = true;
        }
        CTX.lightService.computeSunInfo();
        globalDataUBO.sunPosition = CTX.lightService.getSunPosition();
        globalDataUBO.sunColor = CTX.lightService.getSunColor();
        currentFrame->getResourceAs<BufferInstance>(RID_GLOBAL_DATA)->update(&globalDataUBO);
    }
}
