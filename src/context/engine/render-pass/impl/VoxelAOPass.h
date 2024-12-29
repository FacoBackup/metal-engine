#ifndef VOXELAOPASS_H
#define VOXELAOPASS_H

#include "../AbstractRenderPass.h"
#include "../../../../dto/push-constant/GIPushConstant.h"

namespace Metal {
    class VoxelAOPass final : public AbstractRenderPass {
        GIPushConstant pushConstant{};

    public:
        explicit VoxelAOPass(ApplicationContext &context)
            : AbstractRenderPass(context) {
        }

        PipelineInstance *getPipeline() override;

        void onSync() override;
    };
} // Metal

#endif //VOXELAOPASS_H
