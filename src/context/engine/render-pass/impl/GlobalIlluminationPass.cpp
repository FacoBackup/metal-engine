#include "GlobalIlluminationPass.h"
#include "../../../ApplicationContext.h"

namespace Metal {
    PipelineInstance *GlobalIlluminationPass::getPipeline() {
        return context.corePipelines.globalIlluminationPipeline;
    }

    bool GlobalIlluminationPass::shouldRun() {
        return context.engineRepository.voxelGIEnabled;
    }

    void GlobalIlluminationPass::onSync() {
        pushConstant.giSamplesPerPixel = context.engineRepository.giSamplesPerPixel;
        pushConstant.giBounces = context.engineRepository.giBounces;
        pushConstant.shadowsBaseColor = context.engineRepository.shadowsBaseColor;
        pushConstant.ditheringIntensity = context.engineRepository.ditheringIntensity;
        pushConstant.biasHit = context.engineRepository.voxelHitBias;
        recordPushConstant(&pushConstant);
        recordDrawSimpleInstanced(3, 1);
    }
} // Metal
