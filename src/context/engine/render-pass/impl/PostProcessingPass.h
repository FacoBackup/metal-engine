#ifndef POSTPROCESSINGPASS_H
#define POSTPROCESSINGPASS_H
#include "../AbstractRenderPass.h"

namespace Metal {
    class PostProcessingPass final : public AbstractRenderPass {
    public:
        explicit PostProcessingPass(ApplicationContext &context)
            : AbstractRenderPass(context) {
        }

        void onSync() override;

        PipelineInstance *getPipeline() override;
    };
} // Metal

#endif //POSTPROCESSINGPASS_H
