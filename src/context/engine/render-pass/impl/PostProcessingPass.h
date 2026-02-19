#ifndef POSTPROCESSINGPASS_H
#define POSTPROCESSINGPASS_H
#include "../AbstractRenderPass.h"
#include "../../../../dto/push-constant/PostProcessingPushConstant.h"

namespace Metal {
    class PostProcessingPass final : public AbstractRenderPass {
        PostProcessingPushConstant pushConstant{};
    public:
        explicit PostProcessingPass()
            : AbstractRenderPass() {
        }

        void onInitialize() override;

        void onSync() override;
    };
} // Metal

#endif //POSTPROCESSINGPASS_H
