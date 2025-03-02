#include "EngineContext.h"

#include "../../context/ApplicationContext.h"
#include "../../dto/buffers/MaterialInfo.h"
#include "../../dto/buffers/MaterialInfoData.h"
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
        dispatchTLASUpdate();
    }

    void EngineContext::dispatchTLASUpdate() {
        isTLASReady = false;
    }

    void EngineContext::updateTLASInternal() {
        std::vector<glm::mat4x4> transformations{};
        std::vector<MaterialInfoData> materials{};
        transformations.reserve(rtTLASCount);
        for (auto &tlas: rtTopLevelStructures) {
            auto entity = tlas.id;
            auto &transformComponent = context.worldRepository.transforms.at(entity);
            auto &meshComponent = context.worldRepository.meshes.at(entity);
            tlas.invTransform = inverse(transformComponent.model);
            auto *streamed = context.streamingRepository.streamMaterial(meshComponent.materialId);
            if (streamed != nullptr) {
                materials.push_back(MaterialInfoData{
                    streamed->baseColor,
                    streamed->subsurface,
                    streamed->roughness,
                    streamed->metallic,
                    streamed->specular,
                    streamed->specularTint,
                    streamed->clearcoat,
                    streamed->clearcoatGloss,
                    streamed->anisotropic,
                    streamed->sheen,
                    streamed->sheenTint,
                    streamed->isEmissive
                });
                tlas.materialId = materials.size() - 1;
            } else {
                tlas.materialId = -1;
            }
        }

        context.coreBuffers.rtTLASBuffer->update(rtTopLevelStructures.data(),
                                                 rtTLASCount * sizeof(TLAS));
        context.coreBuffers.rtMaterialData->update(materials.data(),
                                                   materials.size() * sizeof(MaterialInfoData));
    }

    void EngineContext::onSync() {
        if (!isBVHReady) {
            auto bvh = context.bvhBuilderService.buildBVH();
            rtTopLevelStructures = bvh.tlas;
            rtTLASCount = rtTopLevelStructures.size();
            context.coreBuffers.rtBLASBuffer->update(bvh.blas.data(),
                                                     bvh.blas.size() * sizeof(BLAS));
            context.coreBuffers.rtTrianglesBuffer->update(bvh.triangles.data(),
                                                          bvh.triangles.size() * sizeof(RTTriangle));
            isBVHReady = true;
        }

        if (!isTLASReady) {
            updateTLASInternal();
            isTLASReady = true;
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
