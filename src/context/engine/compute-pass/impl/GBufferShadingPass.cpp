#include "./GBufferShadingPass.h"
#include "../../../ApplicationContext.h"
#include "../../../../service/pipeline/PipelineBuilder.h"
#include "../../../../service/texture/TextureInstance.h"

namespace Metal {
    void GBufferShadingPass::onInitialize() {
        PipelineBuilder shadingPipelineBuilder = PipelineBuilder::Of("GBufferShading.comp")
                .addDescriptorSet(context.coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(context.coreDescriptorSets.svoData.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferAlbedo.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferNormal.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferPosition.get())
                .addDescriptorSet(context.coreDescriptorSets.lightsData.get())
                .addDescriptorSet(context.coreDescriptorSets.shadingCompute.get())
                .addDescriptorSet(context.coreDescriptorSets.surfaceCacheCompute.get());
        pipelineInstance = context.pipelineService.createPipeline(shadingPipelineBuilder);
    }

    void GBufferShadingPass::onSync() {
        bool surfaceCacheReset = context.engineContext.isGISettingsUpdated() || context.engineContext.
                                 isLightingDataUpdated();
        if (surfaceCacheReset) {
            clearTexture(context.coreTextures.giSurfaceCache->vkImage);
        }

        if (context.engineRepository.enabledDenoiser) {
            clearTexture(context.coreTextures.shading->vkImage);
            context.engineContext.resetGiAccumulationCount();
        } else if (isFirstRun || context.engineContext.isCameraUpdated() || surfaceCacheReset) {
            clearTexture(context.coreTextures.shading->vkImage);
            context.engineContext.resetGiAccumulationCount();
            isFirstRun = false;
        }

        startWriting(context.coreTextures.shading->vkImage);
        recordImageDispatch(context.coreTextures.shading, 8, 8);
        endWriting(context.coreTextures.shading->vkImage);
    }
} // Metal
