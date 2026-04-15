#include "PathTracerPass.h"
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
#include "engine/resource/RenderTargetInstance.h"
#include "engine/resource/BufferInstance.h"
#include "engine/dto/DescriptorInstance.h"
#include "engine/service/DescriptorSetService.h"
#include "engine/frame-builder/EngineFrame.h"

namespace Metal {
    void PathTracerPass::onInitialize() {
        eventListener([this](const Event &) {
            needsUpdate = true;
        }, "EngineRepository");

        PipelineBuilder builder = PipelineBuilder::OfRayTracing(
                    "rt/PathTracer.rgen",
                    "rt/PathTracer.rmiss",
                    "rt/PathTracer.rchit")
                .setPushConstantsSize(sizeof(PathTracerPushConstant))
                .addBufferBinding(getScopedResourceId(RID_GLOBAL_DATA)) // 0
                .addAccelerationStructureBinding(VK_NULL_HANDLE) // 1
                .addStorageImageBinding(getScopedResourceId(RID_ACCUMULATED_FRAME)) // 2

                .addStorageRenderTargetBinding(getScopedResourceId(RID_GBUFFER_RT), RID_GBUFFER_ALBEDO_EMISSIVE) // 3
                .addStorageRenderTargetBinding(getScopedResourceId(RID_GBUFFER_RT), RID_GBUFFER_ROUGHNESS_METALLIC) // 4
                .addStorageRenderTargetBinding(getScopedResourceId(RID_GBUFFER_RT), RID_GBUFFER_RENDER_INDEX_DEPTH) // 5
                .addStorageRenderTargetBinding(getScopedResourceId(RID_GBUFFER_RT), RID_GBUFFER_NORMAL) // 6

                .addBufferBinding(getScopedResourceId(RID_LIGHT_BUFFER)) // 7
                .addBufferBinding(getScopedResourceId(RID_MATERIAL_DATA_BUFFER)) // 8
                .addBufferBinding(getScopedResourceId(RID_PRIMITIVE_DATA_BUFFER)) // 9
                .addCombinedImageSamplerBinding(vulkanContext->vkTextureSampler, VK_NULL_HANDLE, // 10
                                                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
                .addBufferBinding(getScopedResourceId(RID_RESTIR_RESERVOIR_0)) // 11
                .addBufferBinding(getScopedResourceId(RID_RESTIR_RESERVOIR_1)); // 12
        pipelineInstance = pipelineService->createPipeline(builder);
    }

    bool PathTracerPass::shouldRun() {
        return false;
    }

    void PathTracerPass::onSync() {
        auto *accumulatedFrame = frame->getResourceAs<TextureInstance>(RID_ACCUMULATED_FRAME);
        auto *gBuffer = frame->getResourceAs<RenderTargetInstance>(RID_GBUFFER_RT);

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

        // ReSTIR GI
        pushConstant.restirFrameIndex = frameIndex;
        pushConstant.restirEnabled = engineRepository->restirEnabled;
        frameIndex++;

        recordPushConstant(&pushConstant);

        auto *reservoir0 = frame->getResourceAs<BufferInstance>(RID_RESTIR_RESERVOIR_0);
        auto *reservoir1 = frame->getResourceAs<BufferInstance>(RID_RESTIR_RESERVOIR_1);

        auto *descriptor = pipelineInstance->descriptor;
        for (auto &binding: descriptor->bindings) {
            if (binding.bindingPoint == 11) {
                binding.bufferId = reservoir0->getId();
            } else if (binding.bindingPoint == 12) {
                binding.bufferId = reservoir1->getId();
            }
        }
        descriptorSetService->write(descriptor);

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
