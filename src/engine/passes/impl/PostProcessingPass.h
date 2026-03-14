#ifndef POSTPROCESSINGPASS_H
#define POSTPROCESSINGPASS_H
#include "../AbstractRenderPass.h"
#include "../../dto/PostProcessingPushConstant.h"

namespace Metal {
    class PostProcessingPass final : public AbstractRenderPass {
        PostProcessingPushConstant pushConstant{};
    public:
        void onInitialize() override;

        void onSync() override;
    };
} // Metal

#endif //POSTPROCESSINGPASS_H
