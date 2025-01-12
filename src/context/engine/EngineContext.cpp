#include "EngineContext.h"

#include "../../context/ApplicationContext.h"
#include "../../service/buffer/BufferInstance.h"
#include "../../service/voxel/SVOInstance.h"
#include "../../enum/LevelOfDetail.h"
#include "../../service/camera/Camera.h"
#include "../../service/framebuffer/FrameBufferInstance.h"
#include "../../service/texture/TextureInstance.h"

namespace Metal {
    void EngineContext::onInitialize() {
        context.worldGridService.onSync();
        passesService.onInitialize();
    }

    void EngineContext::updateVoxelData() {
        if (context.worldGridRepository.hasMainTileChanged) {
            unsigned int i = 0;
            for (auto *tile: context.worldGridRepository.getLoadedTiles()) {
                if (tile != nullptr) {
                    const auto *svo = context.streamingRepository.streamSVO(tile->id);
                    if (svo != nullptr) {
                        context.coreDescriptorSets.svoData->addBufferDescriptor(
                            i + 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                            svo->voxelsBuffer);
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
                context.coreDescriptorSets.svoData->write(context.vulkanContext);
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

    void EngineContext::onSync() {
        updateCurrentTime();

        context.transformService.onSync();
        context.worldGridService.onSync();
        context.streamingRepository.onSync();
        context.cameraService.onSync();
        context.voxelizationService.onSync();

        updateGlobalData();
        updateVoxelData();
        updateLights();

        passesService.onSync();

        setLightingDataUpdated(false);
        setCameraUpdated(false);
        setGISettingsUpdated(false);
    }

    void EngineContext::updateLights() {
        if (lightingDataUpdated) {
            int index = 0;
            for (auto &entry: context.worldRepository.lights) {
                if (context.worldRepository.hiddenEntities.contains(entry.first)) {
                    continue;
                }

                auto l = entry.second;
                lights[index] = LightData(
                    l.color * l.intensity,
                    context.worldRepository.transforms.at(entry.first).translation,
                    l.innerRadius,
                    l.outerRadius,
                    l.radius
                );
                index++;
            }
            context.coreBuffers.lights->update(lights.data());
            lightsCount = index;
        }
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
        globalDataUBO.lightsQuantity = lightsCount;
        globalDataUBO.enabledSun = context.engineRepository.atmosphereEnabled;

        globalDataUBO.distortionIntensity = camera.distortionIntensity;
        globalDataUBO.chromaticAberrationIntensity = camera.chromaticAberrationIntensity;
        globalDataUBO.distortionEnabled = camera.distortionEnabled;
        globalDataUBO.chromaticAberrationEnabled = camera.chromaticAberrationEnabled;
        globalDataUBO.vignetteEnabled = camera.vignetteEnabled;
        globalDataUBO.vignetteStrength = camera.vignetteStrength;
        globalDataUBO.giBounces = context.engineRepository.giBounces;
        globalDataUBO.giEnabled = context.engineRepository.giEnabled;
        globalDataUBO.giTileSubdivision = context.engineRepository.giTileSubdivision;
        globalDataUBO.giEmissiveFactor = context.engineRepository.giEmissiveFactor;

        globalDataUBO.debugFlag = ShadingMode::IndexOfValue(context.editorRepository.shadingMode);
        globalDataUBO.surfaceCacheWidth = context.coreTextures.giSurfaceCache->width;
        globalDataUBO.surfaceCacheHeight = context.coreTextures.giSurfaceCache->height;
        globalDataUBO.giAccumulationCount++;
        globalDataUBO.globalFrameCount++;

        if (context.engineRepository.incrementTime) {
            context.engineRepository.elapsedTime += .0005f * context.engineRepository.elapsedTimeSpeed;
        }
        globalDataUBO.sunPosition = glm::vec3(0,
                                              std::cos(context.engineRepository.elapsedTime),
                                              std::sin(context.engineRepository.elapsedTime)) * context.engineRepository
                                    .sunDistance;
        globalDataUBO.sunColor = CalculateSunColor(
                                     globalDataUBO.sunPosition.y / context.engineRepository.sunDistance,
                                     context.engineRepository.nightColor, context.engineRepository.dawnColor,
                                     context.engineRepository.middayColor) * context.engineRepository.sunLightIntensity;
        context.coreBuffers.globalData->update(&globalDataUBO);
    }


    glm::vec3 EngineContext::CalculateSunColor(const float elevation, glm::vec3 &nightColor, glm::vec3 &dawnColor,
                                               glm::vec3 &middayColor) {
        if (elevation <= -0.1f) {
            return nightColor;
        }
        if (elevation <= 0.0f) {
            float t = (elevation + 0.1f) / 0.1f;
            return BlendColors(nightColor, dawnColor, t);
        }
        if (elevation <= 0.5f) {
            float t = elevation / 0.5f;
            return BlendColors(dawnColor, middayColor, t);
        }
        return middayColor;
    }

    glm::vec3 EngineContext::BlendColors(glm::vec3 &c1, glm::vec3 &c2, float t) {
        return {
            (c1.x * (1 - t) + c2.x * t),
            (c1.y * (1 - t) + c2.y * t),
            (c1.z * (1 - t) + c2.z * t)
        };
    }
}
