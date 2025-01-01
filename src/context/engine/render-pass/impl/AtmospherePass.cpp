#include "AtmospherePass.h"

#include "../../../ApplicationContext.h"

namespace Metal {
    bool AtmospherePass::shouldRun() {
        return context.engineRepository.atmosphereEnabled;
    }

    void AtmospherePass::onSync() {
        recordDrawSimpleInstanced(3, 1);
    }

    PipelineInstance * AtmospherePass::getPipeline() {
        return context.corePipelines.atmospherePipeline;
    }
} // Metal
