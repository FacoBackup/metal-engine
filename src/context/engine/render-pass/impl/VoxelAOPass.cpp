#include "VoxelAOPass.h"
#include "../../../ApplicationContext.h"

namespace Metal {
    PipelineInstance *VoxelAOPass::getPipeline() {
        return context.corePipelines.voxelAOPipeline;
    }

    bool VoxelAOPass::shouldRun() {
        return context.engineRepository.voxelGIEnabled;
    }

    void VoxelAOPass::onSync() {
        pushConstant.bias = context.engineRepository.voxelRaytracingBias;
        pushConstant.shadowsBaseColor = context.engineRepository.shadowsBaseColor;
        pushConstant.biasHit = context.engineRepository.voxelHitBias;
        recordPushConstant(&pushConstant);
        recordDrawSimpleInstanced(3, 1);
    }
} // Metal
