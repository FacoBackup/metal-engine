#include "VoxelVisualizerPass.h"
#include "../../../../context/ApplicationContext.h"
#include "../../../../enum/LevelOfDetail.h"
#include "../../../../service/voxel/SVOInstance.h"

namespace Metal {
    bool VoxelVisualizerPass::shouldRun() {
        return context.editorRepository.shadingMode == ShadingMode::VOXELS;
    }

    PipelineInstance *VoxelVisualizerPass::getPipeline() {
        return context.corePipelines.voxelDebugVisualizerPipeline;
    }

    void VoxelVisualizerPass::onSync() {
        settings.randomColors = context.voxelizationRepository.randomColors;
        settings.showRaySearchCount = context.voxelizationRepository.showRaySearchCount;
        settings.showRayTestCount = context.voxelizationRepository.showRayTestCount;
        recordPushConstant(&settings);
        recordDrawSimpleInstanced(3, 1);
    }
} // Metal
