#include "EngineContext.h"

#include "../../context/ApplicationContext.h"
#include "../../service/buffer/BufferInstance.h"
#include "../../service/camera/Camera.h"
#include "../../service/framebuffer/FrameBufferInstance.h"

namespace Metal {
    void EngineContext::onInitialize() {
        context.worldGridService.onSync();
        context.passesService.onInitialize();
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

    void EngineContext::dispatchBVHBuild() {
        isBVHReady = false;
    }

    void EngineContext::updateTransformations() {
        std::vector<glm::mat4x4> transformations{};
        transformations.reserve(rtTLASCount);
        for (auto &tlas: rtTopLevelStructures) {
            auto entity = tlas.id;
            auto &transformComponent = context.worldRepository.transforms.at(entity);
            tlas.invTransform = inverse(transformComponent.model);
        }
        context.coreBuffers.rtTLASBuffer->update(rtTopLevelStructures.data(),
                                                 rtTLASCount * sizeof(TopLevelAccelerationStructure));
    }

    void EngineContext::onSync() {
        if (!isBVHReady) {
            auto bvh = context.bvhBuilderService.buildBVH();
            rtTopLevelStructures = bvh.tlas;
            rtTLASCount = rtTopLevelStructures.size();
            updateTransformations();
            context.coreBuffers.rtBLASBuffer->update(bvh.blas.data(),
                                                     bvh.blas.size() * sizeof(BottomLevelAccelerationStructure));
            context.coreBuffers.rtTrianglesBuffer->update(bvh.triangles.data(),
                                                          bvh.triangles.size() * sizeof(RTTriangle));
            isBVHReady = true;
        }

        updateCurrentTime();

        context.transformService.onSync();
        context.worldGridService.onSync();
        context.streamingRepository.onSync();
        context.cameraService.onSync();

        if (lightVolumeDataNeedsUpdate) {
            context.lightVolumesService.update();
        }
        updateGlobalData();

        context.passesService.onSync();

        setLightVolumeDataNeedsUpdate(false);
        setCameraUpdated(false);
        setGISettingsUpdated(false);
        setWorldChange(false);
    }

    void EngineContext::updateGlobalData() {
        auto &camera = context.worldRepository.camera;
        globalDataUBO.viewMatrix = camera.viewMatrix;
        globalDataUBO.projectionMatrix = camera.projectionMatrix;
        globalDataUBO.projView = camera.projViewMatrix;
        globalDataUBO.invProj = camera.invProjectionMatrix;
        globalDataUBO.invView = camera.invViewMatrix;
        globalDataUBO.cameraWorldPosition = camera.position;
        globalDataUBO.giStrength = context.engineRepository.giStrength;
        globalDataUBO.lightVolumeCount = context.lightVolumesService.getLightVolumeCount();
        globalDataUBO.volumesOffset = context.lightVolumesService.getVolumesOffset();
        globalDataUBO.volumeShadowSteps = context.engineRepository.volumeShadowSteps;
        globalDataUBO.searchCountDivisor = context.editorRepository.voxelSearchCount;
        globalDataUBO.rtTLASCount = rtTLASCount;
        globalDataUBO.isAtmosphereEnabled = context.engineRepository.atmosphereEnabled;

        globalDataUBO.enabledDenoiser = context.engineRepository.enabledDenoiser;
        globalDataUBO.multipleImportanceSampling = context.engineRepository.multipleImportanceSampling;
        globalDataUBO.giMaxAccumulation = context.engineRepository.giMaxAccumulation;
        globalDataUBO.giSamples = context.engineRepository.giSamples;
        globalDataUBO.giBounces = context.engineRepository.giBounces;
        globalDataUBO.giTileSubdivision = context.engineRepository.giTileSubdivision;
        globalDataUBO.giEmissiveFactor = context.engineRepository.giEmissiveFactor;

        globalDataUBO.debugFlag = ShadingMode::IndexOfValue(context.editorRepository.shadingMode);
        globalDataUBO.surfaceCacheWidth = SURFACE_CACHE_RES;
        globalDataUBO.surfaceCacheHeight = SURFACE_CACHE_RES;
        globalDataUBO.giAccumulationCount++;
        globalDataUBO.globalFrameCount++;

        if (context.engineRepository.incrementTime) {
            context.engineRepository.elapsedTime += .0005f * context.engineRepository.elapsedTimeSpeed;
            setGISettingsUpdated(true);
            lightVolumeDataNeedsUpdate = true;
        }
        context.lightVolumesService.computeSunInfo();
        globalDataUBO.sunPosition = context.lightVolumesService.getSunPosition();
        globalDataUBO.sunColor = context.lightVolumesService.getSunColor();
        context.coreBuffers.globalData->update(&globalDataUBO);
    }
}
