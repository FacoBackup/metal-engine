#ifndef VOXELVISUALIZERRENDERPASS_H
#define VOXELVISUALIZERRENDERPASS_H
#include "../AbstractRenderPass.h"

namespace Metal {
    class VoxelVisualizerRenderPass final : public AbstractRenderPass {
    public:
        explicit VoxelVisualizerRenderPass(ApplicationContext &context)
            : AbstractRenderPass(context) {
        }

        void render(VulkanFrameData &frameData) override;
    };
}

#endif
