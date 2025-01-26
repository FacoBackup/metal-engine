#include "./PathTracerPass.h"
#include "../../../ApplicationContext.h"
#include "../../../../service/pipeline/PipelineBuilder.h"
#include "../../../../service/texture/TextureInstance.h"

namespace Metal {
    void PathTracerPass::onInitialize() {
        PipelineBuilder shadingPipelineBuilder = PipelineBuilder::Of("PathTracer.comp")
                .addDescriptorSet(context.coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(context.coreDescriptorSets.svoData.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferAlbedo.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferNormal.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferPosition.get())
                .addDescriptorSet(context.coreDescriptorSets.lightsData.get())
                .addDescriptorSet(context.coreDescriptorSets.currentImageCompute.get())
                .addDescriptorSet(context.coreDescriptorSets.giSurfaceCacheCompute.get());
        pipelineInstance = context.pipelineService.createPipeline(shadingPipelineBuilder);
    }

    void PathTracerPass::onSync() {
        bool surfaceCacheReset = context.engineContext.isGISettingsUpdated() || context.engineContext.
                                 isLightingDataUpdated();
        if (surfaceCacheReset) {
            clearTexture(context.coreTextures.giSurfaceCache->vkImage);
            clearTexture(context.coreTextures.diSurfaceCache->vkImage);
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
