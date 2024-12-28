#include "EngineContext.h"

#include "../../context/ApplicationContext.h"
#include "../../service/buffer/BufferInstance.h"
#include "../../service/voxel/SVOInstance.h"
#include "CommandBufferRecorder.h"
#include "../../enum/LevelOfDetail.h"
#include "render-pass/impl/GBufferShadingPass.h"
#include "render-pass/impl/OpaqueRenderPass.h"
#include "render-pass/impl/PostProcessingPass.h"
#include "render-pass/tools/GridRenderPass.h"
#include "../../service/camera/Camera.h"
#include "render-pass/impl/VoxelVisualizerPass.h"
#include "render-pass/tools/IconsPass.h"

namespace Metal {
    void EngineContext::onInitialize() {
        context.worldGridService.onSync();
        if (context.isDebugMode()) {
            fullScreenRenderPasses.push_back(std::make_unique<GridRenderPass>(context));
        }
        fullScreenRenderPasses.push_back(std::make_unique<GBufferShadingPass>(context));
        if (context.isDebugMode()) {
            fullScreenRenderPasses.push_back(std::make_unique<VoxelVisualizerPass>(context));
            fullScreenRenderPasses.push_back(std::make_unique<IconsPass>(context));
        }
        postProcessingPasses.push_back(std::make_unique<PostProcessingPass>(context));
        gBufferPasses.push_back(std::make_unique<OpaqueRenderPass>(context));
    }

    void EngineContext::updatePostProcessingData() {
        auto &camera = context.worldRepository.camera;
        postProcessingUBO.distortionIntensity = camera.distortionIntensity;
        postProcessingUBO.chromaticAberrationIntensity = camera.chromaticAberrationIntensity;
        postProcessingUBO.distortionEnabled = camera.distortionEnabled;
        postProcessingUBO.chromaticAberrationEnabled = camera.chromaticAberrationEnabled;
        postProcessingUBO.vignetteEnabled = camera.vignetteEnabled;
        postProcessingUBO.vignetteStrength = camera.vignetteStrength;

        context.coreBuffers.postProcessingSettings->update(&postProcessingUBO);
    }

    void EngineContext::updateVoxelData() {
        if (context.worldGridRepository.hasMainTileChanged) {
            unsigned int i = 0;
            for (auto *tile: context.worldGridRepository.getLoadedTiles()) {
                if (tile != nullptr) {
                    const auto *svo = context.streamingRepository.streamSVO(
                        tile->id, LevelOfDetail::OfNumber(context.voxelizationRepository.levelOfDetail));
                    if (svo != nullptr) {
                        context.coreDescriptorSets.svoData->addBufferDescriptor(
                            i + 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                            svo->buffer);
                        tileInfoUBO.tileCenterValid[i] = glm::vec4(tile->x, 0,
                                                                   tile->z, 1);
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

    void EngineContext::onSync() {
        currentTime = Clock::now();
        std::chrono::duration<float> delta = currentTime - previousTime;
        deltaTime = delta.count();
        previousTime = currentTime;

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime.time_since_epoch());
        currentTimeMs = duration.count();

        if (start == -1) {
            start = currentTimeMs;
        }
        context.transformService.onSync();
        context.worldGridService.onSync();
        context.streamingRepository.onSync();
        context.cameraService.onSync();

        updateGlobalData();
        updatePostProcessingData();
        updateVoxelData();
        updateLights();

        context.coreRenderPasses.gBufferPass->recordCommands(gBufferPasses);
        context.coreRenderPasses.fullScreenPass->recordCommands(fullScreenRenderPasses);
        context.coreRenderPasses.postProcessingPass->recordCommands(postProcessingPasses);
    }

    void EngineContext::updateLights() {
        if (hasToUpdateLights) {
            hasToUpdateLights = false;
            int index = 0;
            for (auto &entry: context.worldRepository.lights) {
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
        globalDataUBO.logDepthFC = 2.0f / (std::log(camera.projectionMatrix[0][0] + 1) / std::log(2));
        globalDataUBO.lightsQuantity = lightsCount;

        if (context.atmosphereRepository.incrementTime) {
            context.atmosphereRepository.elapsedTime += .0005f * context.atmosphereRepository.elapsedTimeSpeed;
        }
        globalDataUBO.sunPosition = glm::vec3(std::sin(context.atmosphereRepository.elapsedTime),
                                              std::cos(context.atmosphereRepository.elapsedTime),
                                              0) * context.atmosphereRepository.sunDistance;
        globalDataUBO.sunColor = CalculateSunColor(
            globalDataUBO.sunPosition.y / context.atmosphereRepository.sunDistance,
            context.atmosphereRepository.nightColor, context.atmosphereRepository.dawnColor,
            context.atmosphereRepository.middayColor);
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
