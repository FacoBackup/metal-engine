#include "VoxelAOPass.h"
#include "../../../ApplicationContext.h"

namespace Metal {
    PipelineInstance *VoxelAOPass::getPipeline() {
        return context.corePipelines.voxelAOPipeline;
    }

    bool VoxelAOPass::shouldRun() {
        return context.engineRepository.voxelEnabled;
    }

    void VoxelAOPass::onSync() {
        pushConstant.bias = context.engineRepository.voxelRaytracingBias;
        recordPushConstant(&pushConstant);
        recordDrawSimpleInstanced(3, 1);
    }
} // Metal
