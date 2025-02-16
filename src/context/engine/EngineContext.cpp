#include "EngineContext.h"

#include "../../context/ApplicationContext.h"
#include "../../service/buffer/BufferInstance.h"
#include "../../service/descriptor/DescriptorBinding.h"
#include "../../service/voxel/SVOInstance.h"
#include "../../enum/LevelOfDetail.h"
#include "../../enum/LightVolumeType.h"
#include "../../service/camera/Camera.h"
#include "../../service/framebuffer/FrameBufferInstance.h"
#include "../../service/texture/TextureInstance.h"

namespace Metal {
    void EngineContext::onInitialize() {
        context.worldGridService.onSync();
        context.passesService.onInitialize();
    }

    void EngineContext::updateVoxelData() {
        if (context.worldGridRepository.hasMainTileChanged) {
            unsigned int i = 0;
            std::vector<DescriptorBinding> bindings{};
            for (auto *tile: context.worldGridRepository.getLoadedTiles()) {
                if (tile != nullptr) {
                    const auto *svo = context.streamingRepository.streamSVO(tile->id);
                    if (svo != nullptr) {
                        auto &b = bindings.emplace_back();

                        b.bindingPoint = i + 1;
                        b.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                        b.bufferInstance = svo->voxelsBuffer;

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
                DescriptorInstance::Write(context.vulkanContext, context.coreDescriptorSets.svoData->vkDescriptorSet,
                                          bindings);
                context.coreBuffers.tileInfo->update(tileInfoUBO.tileCenterValid.data());
            }
            context.worldGridRepository.hasMainTileChanged = false;
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

    void EngineContext::dispatchSceneVoxelization() {
        voxelizationRequestId = Util::uuidV4();
    }

    void EngineContext::dispatchBVHBuild() {
        auto bvh = context.bvhBuilderService.buildBVH();
        rtTLASCount = bvh.tlas.size();

        {
            if (trianglesBuffer != nullptr) { trianglesBuffer->dispose(context.vulkanContext); }

            trianglesBuffer = context.bufferService.createBuffer(
                bvh.triangles.size() * sizeof(RTTriangle),
                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

            std::vector<DescriptorBinding> binding{};
            auto &triangles = binding.emplace_back();
            triangles.bindingPoint = 0;
            triangles.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            triangles.bufferInstance = trianglesBuffer;
            trianglesBuffer->update(bvh.triangles.data());
            DescriptorInstance::Write(context.vulkanContext,
                                      context.coreDescriptorSets.rtTrianglesData->vkDescriptorSet,
                                      binding);
        } {
            if (blasBuffer != nullptr) { blasBuffer->dispose(context.vulkanContext); }

            blasBuffer = context.bufferService.createBuffer(
                bvh.triangles.size() * sizeof(BottomLevelAccelerationStructure),
                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

            std::vector<DescriptorBinding> binding{};
            auto &blas = binding.emplace_back();
            blas.bindingPoint = 0;
            blas.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            blas.bufferInstance = blasBuffer;
            blasBuffer->update(bvh.blas.data());
            DescriptorInstance::Write(context.vulkanContext,
                                      context.coreDescriptorSets.rtBLASData->vkDescriptorSet,
                                      binding);
        } {
            if (tlasBuffer != nullptr) { tlasBuffer->dispose(context.vulkanContext); }
            tlasBuffer = context.bufferService.createBuffer(
                bvh.triangles.size() * sizeof(TopLevelAccelerationStructure),
                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

            std::vector<DescriptorBinding> binding{};
            auto &tlas = binding.emplace_back();
            tlas.bindingPoint = 0;
            tlas.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            tlas.bufferInstance = tlasBuffer;
            tlasBuffer->update(bvh.tlas.data());
            DescriptorInstance::Write(context.vulkanContext,
                                      context.coreDescriptorSets.rtTLASData->vkDescriptorSet,
                                      binding);
        }
    }

    void EngineContext::onSync() {
        updateCurrentTime();

        context.transformService.onSync();
        context.worldGridService.onSync();
        context.streamingRepository.onSync();
        context.cameraService.onSync();
        context.voxelizationService.onSync();

        updateVoxelData();
        if (lightVolumeDataNeedsUpdate) {
            context.lightVolumesService.update();
        }
        updateGlobalData();

        context.passesService.onSync();

        setLightVolumeDataNeedsUpdate(false);
        setCameraUpdated(false);
        setGISettingsUpdated(false);
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
