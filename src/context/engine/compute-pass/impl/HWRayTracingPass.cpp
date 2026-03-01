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
                .setPushConstantsSize(sizeof(HWRayTracingPushConstant))
                .addBufferBinding(getScopedResourceId(RID_GLOBAL_DATA))
                .addAccelerationStructureBinding(CTX.rayTracingService.getTLAS())
                .addStorageImageBinding(getScopedResourceId(RID_RAW_RENDERED_FRAME))
                .addStorageImageBinding(getScopedResourceId(RID_GBUFFER_POSITION_INDEX))
                .addStorageImageBinding(getScopedResourceId(RID_GBUFFER_NORMAL))
                .addBufferBinding(getScopedResourceId(RID_LIGHT_BUFFER))
                .addBufferBinding(getScopedResourceId(RID_VOLUMES_BUFFER))
                .addBufferBinding(getScopedResourceId(RID_MATERIAL_BUFFER))
                .addBufferBinding(getScopedResourceId(RID_MESH_METADATA_BUFFER))
                .addCombinedImageSamplerBinding(CTX.vulkanContext.vkImageSampler, VK_NULL_HANDLE,
                                                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1000);
        pipelineInstance = CTX.pipelineService.createPipeline(builder);
    }

    void HWRayTracingPass::onSync() {
        bool anyMeshes = false;
        auto view = CTX.worldRepository.registry.view<MeshComponent, TransformComponent>();
        for (auto entity: view) {
            CTX.streamingService.streamMesh(view.get<MeshComponent>(entity).meshId);
            anyMeshes = true;
        }

        rawRenderedFrame = frame->getResourceAs<TextureInstance>(RID_RAW_RENDERED_FRAME);
        accumulatedFrame = frame->getResourceAs<TextureInstance>(RID_ACCUMULATED_FRAME);
        gBufferPositionIndex = frame->getResourceAs<TextureInstance>(RID_GBUFFER_POSITION_INDEX);
        gBufferNormal = frame->getResourceAs<TextureInstance>(RID_GBUFFER_NORMAL);
        previousColor = frame->getResourceAs<TextureInstance>(RID_PREVIOUS_COLOR);
        previousPositionIndex = frame->getResourceAs<TextureInstance>(RID_PREVIOUS_POSITION_INDEX);
        previousNormal = frame->getResourceAs<TextureInstance>(RID_PREVIOUS_NORMAL);

        bool worldChanged = CTX.engineContext.isUpdateLights() || CTX.worldGridService.isNotFrozen();
        if (isFirstRun || CTX.engineContext.isCameraUpdated() || CTX.engineContext.isGISettingsUpdated() ||
            worldChanged) {
            CTX.worldGridService.freezeVersion();
            clearTexture(rawRenderedFrame->vkImage);
            clearTexture(accumulatedFrame->vkImage);
            CTX.engineContext.resetPathTracerAccumulationCount();
            isFirstRun = false;
        }

        // Copy current to previous before writing new values
        copyTexture(rawRenderedFrame, previousColor);
        copyTexture(gBufferPositionIndex, previousPositionIndex);
        copyTexture(gBufferNormal, previousNormal);

        startWriting(rawRenderedFrame->vkImage);
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
        pushConstant.shouldTrace = CTX.rayTracingService.isReady() && anyMeshes ? 1 : 0;

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
            rawRenderedFrame->width,
            rawRenderedFrame->height,
            1);

        endWriting(rawRenderedFrame->vkImage);
        endWriting(gBufferPositionIndex->vkImage);
        endWriting(gBufferNormal->vkImage);
    }
} // Metal
