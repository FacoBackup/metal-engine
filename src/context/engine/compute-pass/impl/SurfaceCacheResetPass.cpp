#include "SurfaceCacheResetPass.h"
#include "../../../ApplicationContext.h"
#include "../../../../service/pipeline/PipelineBuilder.h"
#include "../../../../service/texture/TextureInstance.h"

namespace Metal {
    void SurfaceCacheResetPass::onInitialize() {
        PipelineBuilder shadingPipelineBuilder = PipelineBuilder::Of("SurfaceCacheReset.comp")
                .addDescriptorSet(context.coreDescriptorSets.diSurfaceCacheCompute.get())
                .addDescriptorSet(context.coreDescriptorSets.diSurfaceCacheComputeVisibility.get());
        pipelineInstance = context.pipelineService.createPipeline(shadingPipelineBuilder);
    }

    bool SurfaceCacheResetPass::shouldRun() {
        return context.engineRepository.enabledDenoiser;
    }

    void SurfaceCacheResetPass::onSync() {
        recordImageDispatch(context.coreTextures.diSurfaceCache, 8, 8);
    }
} // Metal