#ifndef GRIDRENDERPASS_H
#define GRIDRENDERPASS_H
#include "../AbstractRenderPass.h"

namespace Metal {
    class GridRenderPass final : public AbstractRenderPass {
    public:
        explicit GridRenderPass(ApplicationContext &context)
            : AbstractRenderPass(context) {
        }

        PipelineInstance *getPipeline() override;

        bool shouldRun() override;

        void onSync() override;
    };
} // Metal

#endif //GRIDRENDERPASS_H
