#ifndef GRIDRENDERPASS_H
#define GRIDRENDERPASS_H
#include "../../AbstractRenderPass.h"
#include "../../../../../dto/push-constant/GridPushConstant.h"

namespace Metal {
    class GridPass final : public AbstractRenderPass {
        GridPushConstant pushConstant{};
    public:
        explicit GridPass(ApplicationContext &context)
            : AbstractRenderPass(context) {
        }

        PipelineInstance *getPipeline() override;

        bool shouldRun() override;

        void onSync() override;
    };
} // Metal

#endif //GRIDRENDERPASS_H
