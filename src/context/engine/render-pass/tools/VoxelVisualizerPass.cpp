#include "VoxelVisualizerPass.h"
#include "../../../ApplicationContext.h"
#include "../../../../enum/LevelOfDetail.h"
#include "../../../../service/voxel/SVOInstance.h"

namespace Metal {
    bool VoxelVisualizerPass::shouldRun() {
        return context.editorRepository.showVoxels;
    }

    PipelineInstance *VoxelVisualizerPass::getPipeline() {
        return context.corePipelines.voxelDebugVisualizerPipeline;
    }

    void VoxelVisualizerPass::onSync() {
        settings.voxelDebugFlag = context.editorRepository.shadingMode;
        settings.searchCountDivisor = context.editorRepository.voxelSearchCount;
        settings.showRaySearchCount = context.editorRepository.showRaySearchCountVoxels;
        settings.showRayTestCount = context.editorRepository.showRayTestCountVoxels;
        recordPushConstant(&settings);
        recordDrawSimpleInstanced(3, 1);
    }
} // Metal
