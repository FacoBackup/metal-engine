#include "./HWRayTracingPass.h"
#include "../../../ApplicationContext.h"
#include "../../../../service/framebuffer/FrameBufferInstance.h"
#include "../../../../service/pipeline/PipelineBuilder.h"
#include "../../../../service/pipeline/PipelineInstance.h"
#include "../../../../service/texture/TextureInstance.h"
#include "../../../../service/raytracing/RayTracingService.h"
#include "../../../../enum/EngineResourceIDs.h"

namespace Metal {
    void HWRayTracingPass::onInitialize() {
        PipelineBuilder builder = PipelineBuilder::OfRayTracing(
                    "rt/HWRayTracing.rgen",
                    "rt/HWRayTracing.rmiss",
                    "rt/HWRayTracing.rchit")
                .addBufferBinding(getScopedResourceId(RID_GLOBAL_DATA))
                .addAccelerationStructureBinding(CTX.rayTracingService.getTLAS())
                .addFboBinding(getScopedResourceId(RID_G_BUFFER_FBO), 0)
                .addFboBinding(getScopedResourceId(RID_G_BUFFER_FBO), 1)
                .addFboBinding(getScopedResourceId(RID_G_BUFFER_FBO), 2)
                .addStorageImageBinding(getScopedResourceId(RID_RAW_RENDERED_FRAME))
                .addStorageImageBinding(getScopedResourceId(RID_SURFACE_CACHE))
                .addBufferBinding(getScopedResourceId(RID_LIGHT_BUFFER))
                .addBufferBinding(getScopedResourceId(RID_VOLUMES_BUFFER))
                .addBufferBinding(getScopedResourceId(RID_MATERIAL_BUFFER))
                .addCombinedImageSamplerBinding(CTX.vulkanContext.vkImageSampler, VK_NULL_HANDLE,
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
        auto *surfaceCache = frame->getResourceAs<TextureInstance>(RID_SURFACE_CACHE);
        auto *rawRenderedFrame = frame->getResourceAs<TextureInstance>(RID_RAW_RENDERED_FRAME);
        auto *accumulatedFrame = frame->getResourceAs<TextureInstance>(RID_ACCUMULATED_FRAME);

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
        auto *gBuffer = frame->getResourceAs<FrameBufferInstance>(RID_G_BUFFER_FBO);

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
