#include "./HWRayTracingPass.h"
#include "../../../ApplicationContext.h"
#include "../../../../service/framebuffer/FrameBufferInstance.h"
#include "../../../../service/pipeline/PipelineBuilder.h"
#include "../../../../service/pipeline/PipelineInstance.h"
#include "../../../../service/texture/TextureInstance.h"
#include "../../../../service/raytracing/RayTracingService.h"

namespace Metal {
    bool HWRayTracingPass::shouldRun() {
        if (!CTX.rayTracingService.isReady()) {
            return false;
        }

        if (pipelineInstance == nullptr) {
            PipelineBuilder builder = PipelineBuilder::OfRayTracing(
                    "rt/HWRayTracing.rgen",
                    "rt/HWRayTracing.rmiss",
                    "rt/HWRayTracing.rchit")
                .addDescriptorSet(CTX.coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(CTX.coreDescriptorSets.tlasDescriptor.get())
                .addDescriptorSet(CTX.coreDescriptorSets.gBufferAlbedo.get())
                .addDescriptorSet(CTX.coreDescriptorSets.gBufferNormal.get())
                .addDescriptorSet(CTX.coreDescriptorSets.gBufferPosition.get())
                .addDescriptorSet(CTX.coreDescriptorSets.currentFrameDescriptor.get())
                .addDescriptorSet(CTX.coreDescriptorSets.surfaceCacheImage.get())
                .addDescriptorSet(CTX.coreDescriptorSets.lightData.get())
                .addDescriptorSet(CTX.coreDescriptorSets.volumeData.get())
                .addDescriptorSet(CTX.coreDescriptorSets.materialData.get())
                .addDescriptorSet(CTX.coreDescriptorSets.textureArray.get());
            pipelineInstance = CTX.pipelineService.createPipeline(builder);
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
