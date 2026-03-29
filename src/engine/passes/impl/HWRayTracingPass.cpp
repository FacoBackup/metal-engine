#include "HWRayTracingPass.h"
#include "../../dto/PipelineBuilder.h"
#include "../../resource/PipelineInstance.h"
#include "../../resource/TextureInstance.h"
#include "../../service/TLASService.h"
#include "../../service/PipelineService.h"
#include "../../../core/VulkanContext.h"
#include "engine/EngineContext.h"
#include "engine/repository/EngineRepository.h"
#include "editor/enum/EngineResourceIDs.h"
#include "ApplicationEventContext.h"
#include "engine/resource/FrameBufferInstance.h"

namespace Metal {
    void HWRayTracingPass::onInitialize() {
        eventListener([this](const Event &) {
            needsUpdate = true;
        }, "EngineRepository");

        PipelineBuilder builder = PipelineBuilder::OfRayTracing(
                    "rt/HWRayTracing.rgen",
                    "rt/HWRayTracing.rmiss",
                    "rt/HWRayTracing.rchit")
                .setPushConstantsSize(sizeof(HWRayTracingPushConstant))
                .addBufferBinding(getScopedResourceId(RID_GLOBAL_DATA))
                .addAccelerationStructureBinding(VK_NULL_HANDLE)
                .addStorageImageBinding(getScopedResourceId(RID_ACCUMULATED_FRAME))

                .addStorageFboBinding(getScopedResourceId(RID_GBUFFER_FBO), RID_GBUFFER_ALBEDO_EMISSIVE)
                .addStorageFboBinding(getScopedResourceId(RID_GBUFFER_FBO), RID_GBUFFER_ROUGHNESS_METALLIC)
                .addStorageFboBinding(getScopedResourceId(RID_GBUFFER_FBO), RID_GBUFFER_RENDER_INDEX_DEPTH)
                .addStorageFboBinding(getScopedResourceId(RID_GBUFFER_FBO), RID_GBUFFER_NORMAL)

                .addBufferBinding(getScopedResourceId(RID_LIGHT_BUFFER))
                .addBufferBinding(getScopedResourceId(RID_MATERIAL_DATA_BUFFER))
                .addBufferBinding(getScopedResourceId(RID_PRIMITIVE_DATA_BUFFER))
                .addCombinedImageSamplerBinding(vulkanContext->vkImageSampler, VK_NULL_HANDLE,
                                                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        pipelineInstance = pipelineService->createPipeline(builder);
    }

    void HWRayTracingPass::onSync() {
        auto *accumulatedFrame = frame->getResourceAs<TextureInstance>(RID_ACCUMULATED_FRAME);
        auto *gBuffer = frame->getResourceAs<FrameBufferInstance>(RID_GBUFFER_FBO);

        if (isFirstRun || engineContext->isCameraUpdated() || needsUpdate) {
            clearTexture(accumulatedFrame->vkImage);
            engineContext->resetPathTracerAccumulationCount();
            isFirstRun = false;
            needsUpdate = false;
        }

        // Trace rays
        pushConstant.pathTracerMultiplier = engineRepository->pathTracerMultiplier;
        pushConstant.isAtmosphereEnabled = engineRepository->atmosphereEnabled;

        pushConstant.pathTracerSamples = engineRepository->pathTracerSamples;
        pushConstant.pathTracerBounces = engineRepository->pathTracerBounces;
        pushConstant.pathTracingEmissiveFactor = engineRepository->pathTracingEmissiveFactor;

        recordPushConstant(&pushConstant);
        startWriting(accumulatedFrame->vkImage);

        vulkanContext->vkCmdTraceRaysKHR(
            vkCommandBuffer,
            &pipelineInstance->raygenRegion,
            &pipelineInstance->missRegion,
            &pipelineInstance->hitRegion,
            &pipelineInstance->callableRegion,
            gBuffer->bufferWidth,
            gBuffer->bufferHeight,
            1);

        endWriting(accumulatedFrame->vkImage);
    }
} // Metal
