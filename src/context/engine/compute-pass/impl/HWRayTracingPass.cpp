#include "./HWRayTracingPass.h"
#include "../../../ApplicationContext.h"
#include "../../../../service/pipeline/PipelineBuilder.h"
#include "../../../../service/pipeline/PipelineInstance.h"
#include "../../../../service/texture/TextureInstance.h"
#include "../../../../service/raytracing/RayTracingService.h"

namespace Metal {
    bool HWRayTracingPass::shouldRun() {
        if (!ApplicationContext::Get().vulkanContext.rayTracingSupported) {
            return false;
        }

        ApplicationContext::Get().rayTracingService.onSync();

        if (!ApplicationContext::Get().rayTracingService.isReady()) {
            return false;
        }

        if (pipelineInstance == nullptr) {
            PipelineBuilder builder = PipelineBuilder::OfRayTracing(
                    "rt/HWRayTracing.rgen",
                    "rt/HWRayTracing.rmiss",
                    "rt/HWRayTracing.rchit")
                .addDescriptorSet(ApplicationContext::Get().coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(ApplicationContext::Get().coreDescriptorSets.tlasDescriptor.get())
                .addDescriptorSet(ApplicationContext::Get().coreDescriptorSets.gBufferAlbedo.get())
                .addDescriptorSet(ApplicationContext::Get().coreDescriptorSets.gBufferNormal.get())
                .addDescriptorSet(ApplicationContext::Get().coreDescriptorSets.gBufferPosition.get())
                .addDescriptorSet(ApplicationContext::Get().coreDescriptorSets.currentFrameDescriptor.get())
                .addDescriptorSet(ApplicationContext::Get().coreDescriptorSets.surfaceCacheImage.get())
                .addDescriptorSet(ApplicationContext::Get().coreDescriptorSets.lightData.get())
                .addDescriptorSet(ApplicationContext::Get().coreDescriptorSets.volumeData.get());
            pipelineInstance = ApplicationContext::Get().pipelineService.createPipeline(builder);
        }

        return true;
    }

    void HWRayTracingPass::onSync() {
        bool surfaceCacheReset = ApplicationContext::Get().engineContext.isGISettingsUpdated() || ApplicationContext::Get().engineContext.
                              isUpdateLights();
        if (surfaceCacheReset) {
            clearTexture(ApplicationContext::Get().coreTextures.giSurfaceCache->vkImage);
        }

        if (ApplicationContext::Get().engineRepository.enabledDenoiser) {
            clearTexture(ApplicationContext::Get().coreTextures.currentFrame->vkImage);
            ApplicationContext::Get().engineContext.resetPathTracerAccumulationCount();
        } else if (isFirstRun || ApplicationContext::Get().engineContext.isCameraUpdated() || surfaceCacheReset) {
            clearTexture(ApplicationContext::Get().coreTextures.currentFrame->vkImage);
            ApplicationContext::Get().engineContext.resetPathTracerAccumulationCount();
            isFirstRun = false;
        }

        startWriting(ApplicationContext::Get().coreTextures.currentFrame->vkImage);

        // Trace rays
        ApplicationContext::Get().vulkanContext.vkCmdTraceRaysKHR(
            vkCommandBuffer,
            &pipelineInstance->raygenRegion,
            &pipelineInstance->missRegion,
            &pipelineInstance->hitRegion,
            &pipelineInstance->callableRegion,
            ApplicationContext::Get().coreTextures.currentFrame->width,
            ApplicationContext::Get().coreTextures.currentFrame->height,
            1);

        endWriting(ApplicationContext::Get().coreTextures.currentFrame->vkImage);
    }
} // Metal
