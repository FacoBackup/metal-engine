#ifndef UPSCALINGPASS_H
#define UPSCALINGPASS_H
#include "../AbstractRenderPass.h"
#include "../../../../dto/push-constant/UpscalingPushConstant.h"

namespace Metal {
    class UpscalingPass final : public AbstractRenderPass {
        UpscalingPushConstant pushConstant{};
    public:
        explicit UpscalingPass(ApplicationContext &context)
            : AbstractRenderPass(context) {
        }

        void onSync() override;

        PipelineInstance *getPipeline() override;
    };
} // Metal

#endif //UPSCALINGPASS_H
