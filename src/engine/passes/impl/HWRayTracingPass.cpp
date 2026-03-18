#include "HWRayTracingPass.h"
#include "../../dto/PipelineBuilder.h"
#include "../../resource/PipelineInstance.h"
#include "../../resource/TextureInstance.h"
#include "../../service/RayTracingService.h"
#include "../../service/PipelineService.h"
#include "../../../core/VulkanContext.h"
#include "../../../engine/EngineContext.h"
#include "../../../engine/repository/EngineRepository.h"
#include "../../../editor/enum/EngineResourceIDs.h"
#include "../../../editor/service/HistoryEventService.h"

namespace Metal {
    void HWRayTracingPass::onInitialize() {
        eventService->subscribeGeneric([this](const HistoryEvent &) {
            needsUpdate = true;
        });
        eventService->subscribe<EngineRepository>([this](const HistoryEvent &) {
            needsUpdate = true;
        });

        PipelineBuilder builder = PipelineBuilder::OfRayTracing(
                    "rt/HWRayTracing.rgen",
                    "rt/HWRayTracing.rmiss",
                    "rt/HWRayTracing.rchit")
                .setPushConstantsSize(sizeof(HWRayTracingPushConstant))
                .addBufferBinding(getScopedResourceId(RID_GLOBAL_DATA))
                .addAccelerationStructureBinding(rayTracingService->getTLAS())
                .addStorageImageBinding(getScopedResourceId(RID_ACCUMULATED_FRAME))
                .addStorageImageBinding(getScopedResourceId(RID_GBUFFER_POSITION_INDEX))
                .addStorageImageBinding(getScopedResourceId(RID_GBUFFER_NORMAL))
                .addBufferBinding(getScopedResourceId(RID_LIGHT_BUFFER))
                .addBufferBinding(getScopedResourceId(RID_VOLUMES_BUFFER))
                .addBufferBinding(getScopedResourceId(RID_MESH_METADATA_BUFFER))
                .addCombinedImageSamplerBinding(vulkanContext->vkImageSampler, VK_NULL_HANDLE,
                                                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1000);
        pipelineInstance = pipelineService->createPipeline(builder);
    }

    void HWRayTracingPass::onSync() {
        auto *accumulatedFrame = frame->getResourceAs<TextureInstance>(RID_ACCUMULATED_FRAME);
        auto *gBufferPositionIndex = frame->getResourceAs<TextureInstance>(RID_GBUFFER_POSITION_INDEX);
        auto *gBufferNormal = frame->getResourceAs<TextureInstance>(RID_GBUFFER_NORMAL);
        auto *previousPositionIndex = frame->getResourceAs<TextureInstance>(RID_PREVIOUS_POSITION_INDEX);
        auto *previousNormal = frame->getResourceAs<TextureInstance>(RID_PREVIOUS_NORMAL);

        if (isFirstRun || engineContext->isCameraUpdated() || needsUpdate) {
            clearTexture(accumulatedFrame->vkImage);
            engineContext->resetPathTracerAccumulationCount();
            isFirstRun = false;
        }

        copyTexture(gBufferPositionIndex, previousPositionIndex);
        copyTexture(gBufferNormal, previousNormal);

        startWriting(gBufferPositionIndex->vkImage);
        startWriting(gBufferNormal->vkImage);

        // Trace rays
        pushConstant.pathTracerMultiplier = engineRepository->pathTracerMultiplier;
        pushConstant.volumeShadowSteps = engineRepository->volumeShadowSteps;
        pushConstant.isAtmosphereEnabled = engineRepository->atmosphereEnabled;

        pushConstant.pathTracerSamples = engineRepository->pathTracerSamples;
        pushConstant.pathTracerBounces = engineRepository->pathTracerBounces;
        pushConstant.pathTracingEmissiveFactor = engineRepository->pathTracingEmissiveFactor;
        pushConstant.shouldTrace = rayTracingService->isReady();

        pushConstant.dofEnabled = engineRepository->dofEnabled;
        pushConstant.dofFocusDistance = engineRepository->dofFocusDistance;
        pushConstant.dofAperture = engineRepository->dofAperture;
        pushConstant.dofFocalLength = engineRepository->dofFocalLength;

        recordPushConstant(&pushConstant);

        vulkanContext->vkCmdTraceRaysKHR(
            vkCommandBuffer,
            &pipelineInstance->raygenRegion,
            &pipelineInstance->missRegion,
            &pipelineInstance->hitRegion,
            &pipelineInstance->callableRegion,
            accumulatedFrame->width,
            accumulatedFrame->height,
            1);

        endWriting(accumulatedFrame->vkImage);
        endWriting(gBufferPositionIndex->vkImage);
        endWriting(gBufferNormal->vkImage);
    }
} // Metal
