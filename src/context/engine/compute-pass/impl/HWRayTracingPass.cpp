#include "./HWRayTracingPass.h"
#include "../../../ApplicationContext.h"
#include "../../../../service/framebuffer/FrameBufferInstance.h"
#include "../../../../service/pipeline/PipelineBuilder.h"
#include "../../../../service/pipeline/PipelineInstance.h"
#include "../../../../service/texture/TextureInstance.h"
#include "../../../../service/raytracing/RayTracingService.h"

namespace Metal {
    void HWRayTracingPass::onInitialize() {
        PipelineBuilder builder = PipelineBuilder::OfRayTracing(
                    "rt/HWRayTracing.rgen",
                    "rt/HWRayTracing.rmiss",
                    "rt/HWRayTracing.rchit")
                .addResourceBinding(CTX.coreBuffers.globalData)
                .addResourceBinding(CTX.rayTracingService.getTLAS())
                .addResourceBinding(CTX.vulkanContext.vkImageSampler,
                                    CTX.coreFrameBuffers.gBufferFBO->attachments[0]->vkImageView)
                .addResourceBinding(CTX.vulkanContext.vkImageSampler,
                                    CTX.coreFrameBuffers.gBufferFBO->attachments[1]->vkImageView)
                .addResourceBinding(CTX.vulkanContext.vkImageSampler,
                                    CTX.coreFrameBuffers.gBufferFBO->attachments[2]->vkImageView)
                .addResourceBinding(CTX.coreTextures.rawRenderedFrame->vkImageView)
                .addResourceBinding(CTX.coreTextures.surfaceCache->vkImageView)
                .addResourceBinding(CTX.coreBuffers.lightBuffer)
                .addResourceBinding(CTX.coreBuffers.volumesBuffer)
                .addResourceBinding(CTX.coreBuffers.materialBuffer)
                .addResourceBinding(CTX.vulkanContext.vkImageSampler, VK_NULL_HANDLE,
                                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1000);
        pipelineInstance = CTX.pipelineService.createPipeline(builder);
    }

    bool HWRayTracingPass::shouldRun() {
        if (!CTX.rayTracingService.isReady()) {
            return false;
        }

        return true;
    }

    void HWRayTracingPass::onSync() {
        bool surfaceCacheReset = CTX.engineContext.isGISettingsUpdated() || CTX.engineContext.
                                 isUpdateLights();
        if (surfaceCacheReset) {
            clearTexture(CTX.coreTextures.surfaceCache->vkImage);
        }

        if (isFirstRun || CTX.engineContext.isCameraUpdated() || surfaceCacheReset) {
            clearTexture(CTX.coreTextures.rawRenderedFrame->vkImage);
            clearTexture(CTX.coreTextures.accumulatedFrame->vkImage);
            CTX.engineContext.resetPathTracerAccumulationCount();
            isFirstRun = false;
        }

        startWriting(CTX.coreTextures.rawRenderedFrame->vkImage);

        // Trace rays
        CTX.vulkanContext.vkCmdTraceRaysKHR(
            vkCommandBuffer,
            &pipelineInstance->raygenRegion,
            &pipelineInstance->missRegion,
            &pipelineInstance->hitRegion,
            &pipelineInstance->callableRegion,
            CTX.coreFrameBuffers.gBufferFBO->bufferWidth,
            CTX.coreFrameBuffers.gBufferFBO->bufferHeight,
            1);

        endWriting(CTX.coreTextures.rawRenderedFrame->vkImage);
    }
} // Metal
