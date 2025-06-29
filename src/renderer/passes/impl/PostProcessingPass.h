#ifndef POSTPROCESSINGPASS_H
#define POSTPROCESSINGPASS_H
#include "../AbstractRenderPass.h"
#include "../../data/PostProcessingPC.h"

namespace Metal {
    class PostProcessingPass final : public AbstractRenderPass {
        PostProcessingPC pushConstant{};
    public:

        void onInitialize() override;

        void onSync() override;
    };
} // Metal

#endif //POSTPROCESSINGPASS_H
