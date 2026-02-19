#include "./HWRayTracingPass.h"
#include "../../../ApplicationContext.h"
#include "../../../../service/pipeline/PipelineBuilder.h"
#include "../../../../service/pipeline/PipelineInstance.h"
#include "../../../../service/texture/TextureInstance.h"
#include "../../../../service/raytracing/RayTracingService.h"

namespace Metal {
    bool HWRayTracingPass::shouldRun() {
        if (!context.vulkanContext.rayTracingSupported) {
            return false;
        }

        context.rayTracingService.onSync();

        if (!context.rayTracingService.isReady()) {
            return false;
        }

        if (pipelineInstance == nullptr) {
            PipelineBuilder builder = PipelineBuilder::OfRayTracing(
                    "rt/HWRayTracing.rgen",
                    "rt/HWRayTracing.rmiss",
                    "rt/HWRayTracing.rchit")
                .addDescriptorSet(context.coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(context.coreDescriptorSets.tlasDescriptor.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferAlbedo.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferNormal.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferPosition.get())
                .addDescriptorSet(context.coreDescriptorSets.currentFrameDescriptor.get())
                .addDescriptorSet(context.coreDescriptorSets.surfaceCacheImage.get())
                .addDescriptorSet(context.coreDescriptorSets.lightData.get())
                .addDescriptorSet(context.coreDescriptorSets.volumeData.get());
            pipelineInstance = context.pipelineService.createPipeline(builder);
        }

        return true;
    }

    void HWRayTracingPass::onSync() {
        bool surfaceCacheReset = context.engineContext.isGISettingsUpdated() || context.engineContext.
                              isUpdateLights();
        if (surfaceCacheReset) {
            clearTexture(context.coreTextures.giSurfaceCache->vkImage);
        }

        if (context.engineRepository.enabledDenoiser) {
            clearTexture(context.coreTextures.currentFrame->vkImage);
            context.engineContext.resetPathTracerAccumulationCount();
        } else if (isFirstRun || context.engineContext.isCameraUpdated() || surfaceCacheReset) {
            clearTexture(context.coreTextures.currentFrame->vkImage);
            context.engineContext.resetPathTracerAccumulationCount();
            isFirstRun = false;
        }

        startWriting(context.coreTextures.currentFrame->vkImage);

        // Trace rays
        context.vulkanContext.vkCmdTraceRaysKHR(
            vkCommandBuffer,
            &pipelineInstance->raygenRegion,
            &pipelineInstance->missRegion,
            &pipelineInstance->hitRegion,
            &pipelineInstance->callableRegion,
            context.coreTextures.currentFrame->width,
            context.coreTextures.currentFrame->height,
            1);

        endWriting(context.coreTextures.currentFrame->vkImage);
    }
} // Metal
