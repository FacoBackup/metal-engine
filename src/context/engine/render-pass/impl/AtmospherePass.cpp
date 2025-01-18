#include "AtmospherePass.h"

#include "../../../ApplicationContext.h"
#include "../../../../service/pipeline/PipelineBuilder.h"

namespace Metal {
    void AtmospherePass::onInitialize() {
        PipelineBuilder atmosphereBuilder = PipelineBuilder::Of(
               context.coreFrameBuffers.shadingFBO,
               "QUAD.vert",
               "Atmosphere.frag"
           )
           .addDescriptorSet(context.coreDescriptorSets.globalDataDescriptor.get())
           .addDescriptorSet(context.coreDescriptorSets.gBufferPosition.get());
        pipelineInstance = context.pipelineService.createPipeline(atmosphereBuilder);
    }

    bool AtmospherePass::shouldRun() {
        return context.engineRepository.atmosphereEnabled;
    }

    void AtmospherePass::onSync() {
        recordDrawSimpleInstanced(3, 1);
    }
} // Metal
