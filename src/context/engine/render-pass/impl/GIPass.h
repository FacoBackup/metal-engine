#ifndef VOXELAOPASS_H
#define VOXELAOPASS_H

#include "../AbstractRenderPass.h"
#include "../../../../dto/push-constant/GIPushConstant.h"

namespace Metal {
    class GIPass final : public AbstractRenderPass {
        GIPushConstant pushConstant{};

    public:
        explicit GIPass(ApplicationContext &context)
            : AbstractRenderPass(context) {
        }

        PipelineInstance *getPipeline() override;

        bool shouldRun() override;

        void onSync() override;
    };
} // Metal

#endif //VOXELAOPASS_H
