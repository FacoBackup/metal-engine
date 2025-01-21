#include "EngineContext.h"

#include "../../context/ApplicationContext.h"
#include "../../service/buffer/BufferInstance.h"
#include "../../service/voxel/SVOInstance.h"
#include "../../enum/LevelOfDetail.h"
#include "../../enum/LightType.h"
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

        context.passesService.onSync();

        setLightingDataUpdated(false);
        setCameraUpdated(false);
        setGISettingsUpdated(false);
    }

    void EngineContext::registerBespokeLights(int &index) {
        // Register lights
        for (auto &entry: context.worldRepository.lights) {
            if (context.worldRepository.hiddenEntities.contains(entry.first)) {
                continue;
            }
            auto &t = context.worldRepository.transforms.at(entry.first);
            auto &translation = t.translation;
            auto &l = entry.second;

            glm::vec3 normal(0.0f, 1.0f, 0.0f);
            glm::vec3 rotatedNormal = t.rotation * normal;

            lights[index] = LightData(
                l.color * l.intensity,
                translation,
                glm::normalize(rotatedNormal),
                glm::vec3(0),
                l.lightType,
                l.radiusSize
            );
            index++;
        }
    }

    void EngineContext::registerEmissiveLights(int &index) {
        // Register emissive surfaces as light sources
        for (auto &entry: context.worldRepository.meshes) {
            if (context.worldRepository.hiddenEntities.contains(entry.first)) {
                continue;
            }
            auto &translation = context.worldRepository.transforms.at(entry.first).translation;
            auto &mesh = entry.second;
            if (mesh.emissiveSurface) {
                lights[index] = LightData(
                    mesh.albedoColor,
                    // Approximation. The path tracer will sample the albedo color defined in the SVO
                    translation,
                    translation - mesh.emissiveSurfaceArea / 2.f,
                    translation + mesh.emissiveSurfaceArea / 2.f,
                    LightTypes::LightType::EMISSIVE_SURFACE
                );
                index++;
            }
        }
    }

    void EngineContext::registerSun(int &index) {
        if (context.engineRepository.atmosphereEnabled) {
            lights[index] = LightData(
                globalDataUBO.sunColor,
                globalDataUBO.sunPosition,
                globalDataUBO.sunPosition - glm::vec3(context.engineRepository.sunRadius / 2),
                globalDataUBO.sunPosition + glm::vec3(context.engineRepository.sunRadius / 2),
                LightTypes::SPHERE
            );
            index++;
        }
    }

    void EngineContext::updateLights() {
        if (lightingDataUpdated) {
            int index = 0;

            lights.clear();
            lights.reserve(1);
            registerSun(index);

            registerBespokeLights(index);

            registerEmissiveLights(index);

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
        globalDataUBO.lightCount = lightsCount ;
        globalDataUBO.isAtmosphereEnabled = context.engineRepository.atmosphereEnabled;

        globalDataUBO.multipleImportanceSampling = context.engineRepository.multipleImportanceSampling;
        globalDataUBO.giSamples = context.engineRepository.giSamples;
        globalDataUBO.giBounces = context.engineRepository.giBounces;
        globalDataUBO.giTileSubdivision = context.engineRepository.giTileSubdivision;
        globalDataUBO.giEmissiveFactor = context.engineRepository.giEmissiveFactor;

        globalDataUBO.debugFlag = ShadingMode::IndexOfValue(context.editorRepository.shadingMode);
        globalDataUBO.surfaceCacheWidth = context.coreTextures.giSurfaceCache->width;
        globalDataUBO.surfaceCacheHeight = context.coreTextures.giSurfaceCache->height;
        globalDataUBO.giAccumulationCount++;
        globalDataUBO.globalFrameCount++;

        if (context.engineRepository.incrementTime) {
            context.engineRepository.elapsedTime += .0005f * context.engineRepository.elapsedTimeSpeed;
            setGISettingsUpdated(true);
            lightingDataUpdated = true;
        }
        globalDataUBO.sunPosition = glm::vec3(0,
                                              std::cos(context.engineRepository.elapsedTime),
                                              std::sin(context.engineRepository.elapsedTime)) * context.engineRepository.sunDistance;
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
