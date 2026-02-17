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
                .addDescriptorSet(context.coreDescriptorSets.currentFrameDescriptor.get());
            pipelineInstance = context.pipelineService.createPipeline(builder);
        }

        return true;
    }

    void HWRayTracingPass::onSync() {
        auto *outputTex = context.coreTextures.currentFrame;

        clearTexture(outputTex->vkImage);
        startWriting(outputTex->vkImage);

        // Trace rays
        context.vulkanContext.vkCmdTraceRaysKHR(
            vkCommandBuffer,
            &pipelineInstance->raygenRegion,
            &pipelineInstance->missRegion,
            &pipelineInstance->hitRegion,
            &pipelineInstance->callableRegion,
            outputTex->width,
            outputTex->height,
            1);

        endWriting(outputTex->vkImage);
    }
} // Metal
