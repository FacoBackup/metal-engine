#include "EngineContext.h"

#include "../../context/ApplicationContext.h"
#include "../../service/buffer/BufferInstance.h"
#include "../../service/voxel/SVOInstance.h"
#include "CommandBufferRecorder.h"
#include "../../enum/LevelOfDetail.h"
#include "render-pass/impl/GBufferShadingPass.h"
#include "render-pass/impl/OpaqueRenderPass.h"
#include "render-pass/impl/PostProcessingPass.h"
#include "render-pass/tools/GridPass.h"
#include "../../service/camera/Camera.h"
#include "render-pass/impl/AtmospherePass.h"
#include "render-pass/impl/VoxelAOPass.h"
#include "render-pass/tools/VoxelVisualizerPass.h"
#include "render-pass/tools/IconsPass.h"

namespace Metal {
    void EngineContext::onInitialize() {
        context.worldGridService.onSync();
        fullScreenRenderPasses.push_back(std::make_unique<GBufferShadingPass>(context));
        fullScreenRenderPasses.push_back(std::make_unique<AtmospherePass>(context));
        if (context.isDebugMode()) {
            fullScreenRenderPasses.push_back(std::make_unique<GridPass>(context));
            fullScreenRenderPasses.push_back(std::make_unique<VoxelVisualizerPass>(context));
            fullScreenRenderPasses.push_back(std::make_unique<IconsPass>(context));
        }
        aoPass.push_back(std::make_unique<VoxelAOPass>(context));
        postProcessingPasses.push_back(std::make_unique<PostProcessingPass>(context));
        gBufferPasses.push_back(std::make_unique<OpaqueRenderPass>(context));
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
        updateVoxelData();
        updateLights();

        context.coreRenderPasses.aoPass->recordCommands(aoPass);
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

        if (context.engineRepository.incrementTime) {
            context.engineRepository.elapsedTime += .0005f * context.engineRepository.elapsedTimeSpeed;
        }
        globalDataUBO.sunPosition = glm::vec3(std::sin(context.engineRepository.elapsedTime),
                                              std::cos(context.engineRepository.elapsedTime),
                                              0) * context.engineRepository.sunDistance;
        globalDataUBO.sunColor = CalculateSunColor(
            globalDataUBO.sunPosition.y / context.engineRepository.sunDistance,
            context.engineRepository.nightColor, context.engineRepository.dawnColor,
            context.engineRepository.middayColor);
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
