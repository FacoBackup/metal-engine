#include "HWRayTracingPass.h"
#include "../../../ApplicationContext.h"
#include "../../dto/PipelineBuilder.h"
#include "../../resource/PipelineInstance.h"
#include "../../resource/TextureInstance.h"
#include "../../service/RayTracingService.h"
#include "../../../editor/enum/EngineResourceIDs.h"

namespace Metal {
    void HWRayTracingPass::onInitialize() {
        PipelineBuilder builder = PipelineBuilder::OfRayTracing(
                    "rt/HWRayTracing.rgen",
                    "rt/HWRayTracing.rmiss",
                    "rt/HWRayTracing.rchit")
                .setPushConstantsSize(sizeof(HWRayTracingPushConstant))
                .addBufferBinding(getScopedResourceId(RID_GLOBAL_DATA))
                .addAccelerationStructureBinding(CTX.rayTracingService.getTLAS())
                .addStorageImageBinding(getScopedResourceId(RID_ACCUMULATED_FRAME))
                .addStorageImageBinding(getScopedResourceId(RID_GBUFFER_POSITION_INDEX))
                .addStorageImageBinding(getScopedResourceId(RID_GBUFFER_NORMAL))
                .addBufferBinding(getScopedResourceId(RID_LIGHT_BUFFER))
                .addBufferBinding(getScopedResourceId(RID_VOLUMES_BUFFER))
                .addBufferBinding(getScopedResourceId(RID_MESH_METADATA_BUFFER))
                .addCombinedImageSamplerBinding(CTX.vulkanContext.vkImageSampler, VK_NULL_HANDLE,
                                                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1000);
        pipelineInstance = CTX.pipelineService.createPipeline(builder);
    }

    void HWRayTracingPass::onSync() {
        auto *accumulatedFrame = frame->getResourceAs<TextureInstance>(RID_ACCUMULATED_FRAME);
        auto *gBufferPositionIndex = frame->getResourceAs<TextureInstance>(RID_GBUFFER_POSITION_INDEX);
        auto *gBufferNormal = frame->getResourceAs<TextureInstance>(RID_GBUFFER_NORMAL);
        auto *previousPositionIndex = frame->getResourceAs<TextureInstance>(RID_PREVIOUS_POSITION_INDEX);
        auto *previousNormal = frame->getResourceAs<TextureInstance>(RID_PREVIOUS_NORMAL);

        if (isFirstRun || CTX.engineContext.isCameraUpdated() || CTX.engineContext.isGISettingsUpdated()) {
            clearTexture(accumulatedFrame->vkImage);
            CTX.engineContext.resetPathTracerAccumulationCount();
            isFirstRun = false;
        }

        copyTexture(gBufferPositionIndex, previousPositionIndex);
        copyTexture(gBufferNormal, previousNormal);

        startWriting(gBufferPositionIndex->vkImage);
        startWriting(gBufferNormal->vkImage);

        // Trace rays
        pushConstant.pathTracerMultiplier = CTX.engineRepository.pathTracerMultiplier;
        pushConstant.volumeShadowSteps = CTX.engineRepository.volumeShadowSteps;
        pushConstant.isAtmosphereEnabled = CTX.engineRepository.atmosphereEnabled;

        pushConstant.multipleImportanceSampling = CTX.engineRepository.multipleImportanceSampling;
        pushConstant.pathTracerSamples = CTX.engineRepository.pathTracerSamples;
        pushConstant.pathTracerBounces = CTX.engineRepository.pathTracerBounces;
        pushConstant.pathTracingEmissiveFactor = CTX.engineRepository.pathTracingEmissiveFactor;
        pushConstant.shouldTrace = CTX.rayTracingService.isReady();

        pushConstant.dofEnabled = CTX.engineRepository.dofEnabled;
        pushConstant.dofFocusDistance = CTX.engineRepository.dofFocusDistance;
        pushConstant.dofAperture = CTX.engineRepository.dofAperture;
        pushConstant.dofFocalLength = CTX.engineRepository.dofFocalLength;

        recordPushConstant(&pushConstant);

        CTX.vulkanContext.vkCmdTraceRaysKHR(
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
