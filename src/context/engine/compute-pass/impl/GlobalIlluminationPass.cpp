#include "GlobalIlluminationPass.h"
#include "../../../ApplicationContext.h"
#include "../../../../service/texture/TextureInstance.h"

#include "../../../../service/pipeline/PipelineBuilder.h"

namespace Metal {
    void GlobalIlluminationPass::onInitialize() {
        PipelineBuilder giBuilder = PipelineBuilder::Of("GlobalIllumination.comp")
                .addDescriptorSet(context.coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(context.coreDescriptorSets.svoData.get())
                .addDescriptorSet(context.coreDescriptorSets.lightsData.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferAlbedo.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferNormal.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferPosition.get())
                .addDescriptorSet(context.coreDescriptorSets.surfaceCacheCompute.get())
                .addDescriptorSet(context.coreDescriptorSets.giCompute.get());
        pipelineInstance = context.pipelineService.createPipeline(giBuilder);
    }

    void GlobalIlluminationPass::onSync() {
        bool surfaceCacheReset = context.engineContext.isGISettingsUpdated() || context.engineContext.
                                 isLightingDataUpdated();
        if (surfaceCacheReset) {
            clearTexture(context.coreTextures.giSurfaceCache->vkImage);
        }
        if (isFirstRun || context.engineContext.isCameraUpdated() || surfaceCacheReset) {
            clearTexture(context.coreTextures.globalIllumination->vkImage);
            context.engineContext.resetGiAccumulationCount();
            isFirstRun = false;
        }

        if (!context.engineRepository.giEnabled) {
            return;
        }

        startWriting(context.coreTextures.globalIllumination->vkImage);
        recordImageDispatch(context.coreTextures.globalIllumination, 8, 8);
        endWriting(context.coreTextures.globalIllumination->vkImage);
    }
} // Metal
