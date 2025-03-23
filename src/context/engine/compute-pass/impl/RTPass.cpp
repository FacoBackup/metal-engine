#include "./RTPass.h"
#include "../../../ApplicationContext.h"
#include "../../../../service/pipeline/PipelineBuilder.h"
#include "../../../../service/texture/TextureInstance.h"

namespace Metal {
    void RTPass::onInitialize() {
        PipelineBuilder shadingPipelineBuilder = PipelineBuilder::Of("rt/PathTrace.comp")
                .addDescriptorSet(context.coreDescriptorSets.rtDescriptorSet.get())
                .addDescriptorSet(context.coreDescriptorSets.lightsBuffer.get())
                .addDescriptorSet(context.coreDescriptorSets.volumesBuffer.get());

        pipelineInstance = context.pipelineService.createPipeline(shadingPipelineBuilder);
    }

    void RTPass::onSync() {
        bool surfaceCacheReset = context.engineContext.hasWorldChanged() || context.engineContext.isGISettingsUpdated()
                                 || context.engineContext.isLightingDataUpdated();
        if (surfaceCacheReset) {
            clearTexture(context.coreTextures.giSurfaceCache->vkImage);
        }


        if (context.engineRepository.enabledDenoiser) {
            clearTexture(context.coreTextures.currentFrame->vkImage);
            context.engineContext.resetAccumulationCount();
        } else if (isFirstRun || context.engineContext.isCameraUpdated() || surfaceCacheReset) {
            clearTexture(context.coreTextures.currentFrame->vkImage);
            context.engineContext.resetAccumulationCount();
            isFirstRun = false;
        }

        if (!context.engineRepository.enabledDenoiser && context.engineRepository.maxAccumulation <= context.engineContext.getAccumulationCount()) {
            return;
        }
        clearTexture(context.coreTextures.currentPositions->vkImage, {0,0,0,0});

        startWriting(context.coreTextures.currentFrame->vkImage);
        recordImageDispatch(context.coreTextures.currentFrame, 8, 8);
    }
} // Metal
