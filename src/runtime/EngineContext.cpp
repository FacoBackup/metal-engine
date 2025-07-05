#include "EngineContext.h"

#include "../MetalContextProvider.h"
#include "../renderer/data/MaterialInfo.h"
#include "../renderer/data/MaterialInfoData.h"
#include "../renderer/data/BufferInstance.h"
#include "data/Camera.h"
#include "../renderer/data/FrameBufferInstance.h"

namespace Metal
{
    void EngineContext::onInitialize()
    {
        SINGLETONS.passesService.onInitialize();
    }

    void EngineContext::updateCurrentTime()
    {
        currentTime = Clock::now();
        std::chrono::duration<float> delta = currentTime - previousTime;
        deltaTime = delta.count();
        previousTime = currentTime;

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime.time_since_epoch());
        currentTimeMs = duration.count();

        if (start == -1)
        {
            start = currentTimeMs;
        }
    }

    void EngineContext::dispatchBVHBuild()
    {
        isBVHReady = false;
        dispatchTLASUpdate();
    }

    void EngineContext::dispatchTLASUpdate()
    {
        isTLASReady = false;
    }

    void EngineContext::updateTLASInternal()
    {
        std::vector<glm::mat4x4> transformations{};
        std::vector<MaterialInfoData> materials{};
        transformations.reserve(rtTLASCount);
        for (auto& tlas : rtTopLevelStructures)
        {
            auto entity = tlas.id;
            if (!SINGLETONS.worldRepository.transforms.contains(entity))
            {
                continue;
            }
            auto& transformComponent = SINGLETONS.worldRepository.transforms.at(entity);
            auto& meshComponent = SINGLETONS.worldRepository.meshes.at(entity);
            tlas.invTransform = inverse(transformComponent.model);
            auto* streamed = SINGLETONS.streamingRepository.streamMaterial(meshComponent.materialId);
            if (streamed != nullptr)
            {
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
                    streamed->isEmissive,
                    streamed->transmission,
                    streamed->ior,
                    streamed->absorption
                });
                tlas.materialId = materials.size() - 1;
            }
            else
            {
                tlas.materialId = -1;
            }
        }

        SINGLETONS.coreBuffers.rtTLASBuffer->update(rtTopLevelStructures.data(),
                                                 rtTLASCount * sizeof(TLASBuffer));
        SINGLETONS.coreBuffers.rtMaterialData->update(materials.data(),
                                                   materials.size() * sizeof(MaterialInfoData));
    }

    void EngineContext::onSync()
    {
        if (!isBVHReady)
        {
            auto bvh = SINGLETONS.bvhBuilderService.buildBVH();
            rtTopLevelStructures = bvh.tlas;
            rtTLASCount = rtTopLevelStructures.size();
            SINGLETONS.coreBuffers.rtBLASBuffer->update(bvh.blas.data(),
                                                     bvh.blas.size() * sizeof(BLASBuffer));
            SINGLETONS.coreBuffers.rtTrianglesBuffer->update(bvh.triangles.data(),
                                                          bvh.triangles.size() * sizeof(TriangleBuffer));
            isBVHReady = true;
        }

        if (!isTLASReady)
        {
            updateTLASInternal();
            isTLASReady = true;
        }

        updateCurrentTime();

        SINGLETONS.transformService.onSync();
        SINGLETONS.streamingRepository.onSync();
        SINGLETONS.cameraService.onSync();

        if (lightVolumeDataNeedsUpdate)
        {
            SINGLETONS.lightsService.update();
            SINGLETONS.volumesService.update();
        }
        SINGLETONS.windowService.updateBuffer();
        updateGlobalData();

        SINGLETONS.passesService.onSync();

        SINGLETONS.windowService.reset();

        setLightVolumeDataNeedsUpdate(false);
        setCameraUpdated(false);
        setGISettingsUpdated(false);
        setWorldChange(false);
    }

    void EngineContext::updateGlobalData()
    {
        SINGLETONS.lightsService.computeSunInfo();

        auto& camera = SINGLETONS.worldRepository.camera;
        globalDataUBO.viewMatrix = camera.viewMatrix;
        globalDataUBO.projectionMatrix = camera.projectionMatrix;
        globalDataUBO.projView = camera.projViewMatrix;
        globalDataUBO.invProj = camera.invProjectionMatrix;
        globalDataUBO.invView = camera.invViewMatrix;
        globalDataUBO.cameraWorldPosition = camera.position;
        globalDataUBO.giStrength = SINGLETONS.engineRepository.giStrength;
        globalDataUBO.lightCount = SINGLETONS.lightsService.getLightCount();
        globalDataUBO.volumeCount = SINGLETONS.volumesService.getVolumeCount();
        globalDataUBO.volumeShadowSteps = SINGLETONS.engineRepository.volumeShadowSteps;
        globalDataUBO.rtTLASCount = rtTLASCount;
        globalDataUBO.isAtmosphereEnabled = SINGLETONS.engineRepository.atmosphereEnabled;

        globalDataUBO.enabledDenoiser = SINGLETONS.engineRepository.enabledDenoiser;
        globalDataUBO.multipleImportanceSampling = SINGLETONS.engineRepository.multipleImportanceSampling;
        globalDataUBO.maxAccumulation = SINGLETONS.engineRepository.maxAccumulation;
        globalDataUBO.giSamples = SINGLETONS.engineRepository.giSamples;
        globalDataUBO.giBounces = SINGLETONS.engineRepository.giBounces;
        globalDataUBO.giTileSubdivision = SINGLETONS.engineRepository.giTileSubdivision;
        globalDataUBO.surfaceCacheMinSamples = SINGLETONS.engineRepository.surfaceCacheMinSamples;
        globalDataUBO.giEmissiveFactor = SINGLETONS.engineRepository.giEmissiveFactor;
        accumulationCount++;

        globalDataUBO.debugFlag = ShadingMode::IndexOfValue(SINGLETONS.editorRepository.shadingMode);
        globalDataUBO.surfaceCacheWidth = SURFACE_CACHE_RES;
        globalDataUBO.surfaceCacheHeight = SURFACE_CACHE_RES;
        globalDataUBO.globalFrameCount++;

        if (SINGLETONS.engineRepository.incrementTime)
        {
            SINGLETONS.engineRepository.elapsedTime += .0005f * SINGLETONS.engineRepository.elapsedTimeSpeed;
            setGISettingsUpdated(true);
            lightVolumeDataNeedsUpdate = true;
        }
        globalDataUBO.sunPosition = SINGLETONS.lightsService.getSunPosition();
        globalDataUBO.sunColor = SINGLETONS.lightsService.getSunColor();
        SINGLETONS.coreBuffers.globalData->update(&globalDataUBO);
    }
}
