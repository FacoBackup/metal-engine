#ifndef VOXELVISUALIZERPASS_H
#define VOXELVISUALIZERPASS_H

#include "../../AbstractRenderPass.h"
#include "../../../../../dto/push-constant/VoxelDebugSettingsPushConstant.h"

namespace Metal {
    class VoxelVisualizerPass final : public AbstractRenderPass {
        VoxelDebugSettingsPushConstant settings{};

    public:
        explicit VoxelVisualizerPass(ApplicationContext &context)
            : AbstractRenderPass(context) {
        }

        void onInitialize() override;

        bool shouldRun() override;

        void onSync() override;
    };
} // Metal

#endif //VOXELVISUALIZERPASS_H
