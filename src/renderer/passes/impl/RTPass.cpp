#include "RTPass.h"
#include "../../../MetalContextProvider.h"
#include "../../data/PipelineBuilder.h"
#include "../../data/TextureInstance.h"

namespace Metal {
    void RTPass::onInitialize() {
        PipelineBuilder shadingPipelineBuilder = PipelineBuilder::Of("rt/PathTrace.comp")
                .addDescriptorSet(SINGLETONS.coreDescriptorSets.rtDescriptorSet.get())
                .addDescriptorSet(SINGLETONS.coreDescriptorSets.lightsBuffer.get())
                .addDescriptorSet(SINGLETONS.coreDescriptorSets.volumesBuffer.get());

        pipelineInstance = SINGLETONS.pipelineService.createPipeline(shadingPipelineBuilder);
    }

    void RTPass::onSync() {
        bool surfaceCacheReset = SINGLETONS.engineContext.hasWorldChanged() || SINGLETONS.engineContext.isGISettingsUpdated()
                                 || SINGLETONS.engineContext.isLightingDataUpdated();
        if (surfaceCacheReset) {
            clearTexture(SINGLETONS.coreTextures.giSurfaceCache->vkImage);
        }


        if (SINGLETONS.engineRepository.enabledDenoiser) {
            clearTexture(SINGLETONS.coreTextures.current1Frame->vkImage);
            SINGLETONS.engineContext.resetAccumulationCount();
        } else if (isFirstRun || SINGLETONS.engineContext.isCameraUpdated() || surfaceCacheReset) {
            clearTexture(SINGLETONS.coreTextures.current1Frame->vkImage);
            SINGLETONS.engineContext.resetAccumulationCount();
            isFirstRun = false;
        }

        if (!SINGLETONS.engineRepository.enabledDenoiser && SINGLETONS.engineRepository.maxAccumulation <= SINGLETONS.engineContext.getAccumulationCount()) {
            return;
        }
        clearTexture(SINGLETONS.coreTextures.currentPositions->vkImage, {0,0,0,0});

        startWriting(SINGLETONS.coreTextures.current1Frame->vkImage);
        recordImageDispatch(SINGLETONS.coreTextures.current1Frame, 8, 8);
    }
} // Metal
