#include "./HWRayTracingPass.h"
#include "../../../ApplicationContext.h"
#include "../../../../service/framebuffer/FrameBufferInstance.h"
#include "../../../../service/pipeline/PipelineBuilder.h"
#include "../../../../service/pipeline/PipelineInstance.h"
#include "../../../../service/texture/TextureInstance.h"
#include "../../../../service/raytracing/RayTracingService.h"

namespace Metal {
    void HWRayTracingPass::onInitialize() {
        auto *gBuffer = CTX.framebufferService.getResource("gBufferFBO");
        PipelineBuilder builder = PipelineBuilder::OfRayTracing(
                    "rt/HWRayTracing.rgen",
                    "rt/HWRayTracing.rmiss",
                    "rt/HWRayTracing.rchit")
                .addResourceBinding("globalData")
                .addResourceBinding(CTX.rayTracingService.getTLAS())
                .addResourceBinding(CTX.vulkanContext.vkImageSampler,
                                    gBuffer->attachments[0]->vkImageView)
                .addResourceBinding(CTX.vulkanContext.vkImageSampler,
                                    gBuffer->attachments[1]->vkImageView)
                .addResourceBinding(CTX.vulkanContext.vkImageSampler,
                                    gBuffer->attachments[2]->vkImageView)
                .addResourceBinding(CTX.textureService.getResource("rawRenderedFrame")->vkImageView)
                .addResourceBinding(CTX.textureService.getResource("surfaceCache")->vkImageView)
                .addResourceBinding("lightBuffer")
                .addResourceBinding("volumesBuffer")
                .addResourceBinding("materialBuffer")
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
        auto *surfaceCache = CTX.textureService.getResource("surfaceCache");
        auto *rawRenderedFrame = CTX.textureService.getResource("rawRenderedFrame");
        auto *accumulatedFrame = CTX.textureService.getResource("accumulatedFrame");

        bool surfaceCacheReset = CTX.engineContext.isGISettingsUpdated() || CTX.engineContext.
                                 isUpdateLights();
        if (surfaceCacheReset) {
            clearTexture(surfaceCache->vkImage);
        }

        if (isFirstRun || CTX.engineContext.isCameraUpdated() || surfaceCacheReset) {
            clearTexture(rawRenderedFrame->vkImage);
            clearTexture(accumulatedFrame->vkImage);
            CTX.engineContext.resetPathTracerAccumulationCount();
            isFirstRun = false;
        }

        startWriting(rawRenderedFrame->vkImage);
        auto *gBuffer = CTX.framebufferService.getResource("gBufferFBO");

        // Trace rays
        CTX.vulkanContext.vkCmdTraceRaysKHR(
            vkCommandBuffer,
            &pipelineInstance->raygenRegion,
            &pipelineInstance->missRegion,
            &pipelineInstance->hitRegion,
            &pipelineInstance->callableRegion,
            gBuffer->bufferWidth,
            gBuffer->bufferHeight,
            1);

        endWriting(rawRenderedFrame->vkImage);
    }
} // Metal
