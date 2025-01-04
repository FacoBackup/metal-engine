#include "GIPass.h"
#include "../../../ApplicationContext.h"

namespace Metal {
    PipelineInstance *GIPass::getPipeline() {
        return context.corePipelines.globalIlluminationPipeline;
    }

    bool GIPass::shouldRun() {
        return context.engineRepository.giEnabled;
    }

    void GIPass::onSync() {
        pushConstant.giSamplesPerPixel = context.engineRepository.giSamplesPerPixel;
        pushConstant.giBounces = context.engineRepository.giBounces;
        pushConstant.ditheringIntensity = context.engineRepository.giDitheringIntensity;
        recordPushConstant(&pushConstant);
        recordDrawSimpleInstanced(3, 1);
    }
} // Metal
