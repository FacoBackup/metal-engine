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
    void EngineContext::resetPathTracerAccumulationCount() const {
        ApplicationContext::Get().engineRepository.pathTracerAccumulationCount = 0;
    }

    void EngineContext::onInitialize() {
        ApplicationContext::Get().worldGridService.onSync();
        ApplicationContext::Get().passesService.onInitialize();
    }

    void EngineContext::updateVoxelData() {
        if (ApplicationContext::Get().worldGridRepository.hasMainTileChanged) {
            unsigned int i = 0;
            std::vector<DescriptorBinding> bindings{};
            for (auto *tile: ApplicationContext::Get().worldGridRepository.getLoadedTiles()) {
                if (tile != nullptr) {
                    const auto *svo = ApplicationContext::Get().streamingRepository.streamSVO(tile->id);
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
                DescriptorInstance::Write(ApplicationContext::Get().coreDescriptorSets.svoData->vkDescriptorSet,
                                          bindings);
                ApplicationContext::Get().coreBuffers.tileInfo->update(tileInfoUBO.tileCenterValid.data());
            }
            ApplicationContext::Get().worldGridRepository.hasMainTileChanged = false;
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

    void EngineContext::onSync() {
        updateCurrentTime();

        ApplicationContext::Get().transformService.onSync();
        ApplicationContext::Get().worldGridService.onSync();
        ApplicationContext::Get().streamingRepository.onSync();
        ApplicationContext::Get().cameraService.onSync();


        updateVoxelData();
        if (updateLights) {
            ApplicationContext::Get().lightService.onSync();
        }

        if (updateVolumes) {
            ApplicationContext::Get().volumeService.onSync();
        }
        updateGlobalData();

        ApplicationContext::Get().passesService.onSync();

        setUpdateLights(false);
        setCameraUpdated(false);
        setGISettingsUpdated(false);

        ApplicationContext::Get().videoExporterService.onSync();
    }

    void EngineContext::updateGlobalData() {
        auto &camera = ApplicationContext::Get().worldRepository.camera;
        auto *fbo = ApplicationContext::Get().coreFrameBuffers.postProcessingFBO;
        globalDataUBO.outputRes.x = fbo->bufferWidth;
        globalDataUBO.outputRes.y = fbo->bufferHeight;
        globalDataUBO.viewMatrix = camera.viewMatrix;
        globalDataUBO.projectionMatrix = camera.projectionMatrix;
        globalDataUBO.projView = camera.projViewMatrix;
        globalDataUBO.invProj = camera.invProjectionMatrix;
        globalDataUBO.invView = camera.invViewMatrix;
        globalDataUBO.cameraWorldPosition = camera.position;
        globalDataUBO.pathTracerMultiplier = ApplicationContext::Get().engineRepository.pathTracerMultiplier;
        globalDataUBO.volumeCount = ApplicationContext::Get().volumeService.getCount();
        globalDataUBO.lightsCount = ApplicationContext::Get().lightService.getCount();
        globalDataUBO.volumeShadowSteps = ApplicationContext::Get().engineRepository.volumeShadowSteps;
        globalDataUBO.isAtmosphereEnabled = ApplicationContext::Get().engineRepository.atmosphereEnabled;

        globalDataUBO.enabledDenoiser = ApplicationContext::Get().engineRepository.enabledDenoiser;
        globalDataUBO.multipleImportanceSampling = ApplicationContext::Get().engineRepository.multipleImportanceSampling;
        globalDataUBO.pathTracerMaxSamples = ApplicationContext::Get().engineRepository.pathTracerMaxSamples;
        globalDataUBO.pathTracerSamples = ApplicationContext::Get().engineRepository.pathTracerSamples;
        globalDataUBO.pathTracerBounces = ApplicationContext::Get().engineRepository.pathTracerBounces;
        globalDataUBO.giTileSubdivision = ApplicationContext::Get().engineRepository.giTileSubdivision;
        globalDataUBO.giEmissiveFactor = ApplicationContext::Get().engineRepository.giEmissiveFactor;

        globalDataUBO.debugFlag = ShadingMode::IndexOfValue(ApplicationContext::Get().editorRepository.shadingMode);
        globalDataUBO.surfaceCacheWidth = SURFACE_CACHE_RES;
        globalDataUBO.surfaceCacheHeight = SURFACE_CACHE_RES;
        ApplicationContext::Get().engineRepository.pathTracerAccumulationCount++;
        globalDataUBO.pathTracerAccumulationCount = ApplicationContext::Get().engineRepository.pathTracerAccumulationCount;
        globalDataUBO.globalFrameCount++;

        if (ApplicationContext::Get().engineRepository.incrementTime) {
            ApplicationContext::Get().engineRepository.elapsedTime += .0005f * ApplicationContext::Get().engineRepository.elapsedTimeSpeed;
            setGISettingsUpdated(true);
            updateLights = true;
        }
        ApplicationContext::Get().lightService.computeSunInfo();
        globalDataUBO.sunPosition = ApplicationContext::Get().lightService.getSunPosition();
        globalDataUBO.sunColor = ApplicationContext::Get().lightService.getSunColor();
        ApplicationContext::Get().coreBuffers.globalData->update(&globalDataUBO);
    }
}
