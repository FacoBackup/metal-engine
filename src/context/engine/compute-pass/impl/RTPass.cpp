#include "./RTPass.h"
#include "../../../ApplicationContext.h"
#include "../../../../service/pipeline/PipelineBuilder.h"
#include "../../../../service/texture/TextureInstance.h"

namespace Metal {
    void RTPass::onInitialize() {
        PipelineBuilder shadingPipelineBuilder = PipelineBuilder::Of("rt/PathTrace.comp")
                .addDescriptorSet(context.coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(context.coreDescriptorSets.rtTrianglesData.get())
                .addDescriptorSet(context.coreDescriptorSets.rtBLASData.get())
                .addDescriptorSet(context.coreDescriptorSets.rtTLASData.get())
                .addDescriptorSet(context.coreDescriptorSets.rtTransformationData.get())
                .addDescriptorSet(context.coreDescriptorSets.currentFrameDescriptor.get());
        pipelineInstance = context.pipelineService.createPipeline(shadingPipelineBuilder);
    }

    void RTPass::onSync() {
        bool surfaceCacheReset = context.engineContext.isGISettingsUpdated() || context.engineContext.
                                 isLightingDataUpdated();
        if (surfaceCacheReset) {
            clearTexture(context.coreTextures.giSurfaceCache->vkImage);
        }

        if (context.engineRepository.enabledDenoiser) {
            clearTexture(context.coreTextures.currentFrame->vkImage);
            context.engineContext.resetGiAccumulationCount();
        } else if (isFirstRun || context.engineContext.isCameraUpdated() || surfaceCacheReset) {
            clearTexture(context.coreTextures.currentFrame->vkImage);
            context.engineContext.resetGiAccumulationCount();
            isFirstRun = false;
        }

        startWriting(context.coreTextures.currentFrame->vkImage);
        recordImageDispatch(context.coreTextures.currentFrame, 8, 8);
    }
} // Metal
