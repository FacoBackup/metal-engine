#include "VoxelVisualizerPass.h"
#include "../../../../context/ApplicationContext.h"
#include "../../../../service/voxel/SVOInstance.h"

namespace Metal {
    bool VoxelVisualizerPass::shouldRun() {
        return context.editorRepository.shadingMode == ShadingMode::VOXELS;
    }

    PipelineInstance *VoxelVisualizerPass::getPipeline() {
        return context.corePipelines.voxelDebugVisualizerPipeline;
    }

    void VoxelVisualizerPass::onSync() {
        // TODO - Move this to a tile disposal system, once a tile gets offloaded than dispose SVO and when loading new tile, stream SVO
        if (context.worldGridRepository.getCurrentTile()->id != currentTile) {
            const auto *svo = context.svoService.create(context.worldGridRepository.getCurrentTile()->id);
            if (svo != nullptr) {
                context.coreDescriptorSets.svoData->addBufferDescriptor(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                                                        svo->buffer);
                context.coreDescriptorSets.svoData->write(context.vulkanContext);
                hasData = true;
            }
        }
        if (hasData) {
            currentTile = context.worldGridRepository.getCurrentTile()->id;

            settings.showRaySearchCount = context.voxelizationRepository.randomColors;
            settings.showRaySearchCount = context.voxelizationRepository.showRaySearchCount;
            settings.showRaySearchCount = context.voxelizationRepository.showRayTestCount;
            recordPushConstant(&settings);
            recordDrawSimpleInstanced(3, 1);
        }
    }
} // Metal
