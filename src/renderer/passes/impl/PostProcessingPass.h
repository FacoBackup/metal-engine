#ifndef POSTPROCESSINGPASS_H
#define POSTPROCESSINGPASS_H
#include "../AbstractComputePass.h"
#include "../AbstractRenderPass.h"
#include "../../data/PostProcessingPC.h"

namespace Metal {
    class PostProcessingPass final : public AbstractComputePass {
        PostProcessingPC pushConstant{};
    public:

        void onInitialize() override;

        void onSync() override;
    };
} // Metal

#endif //POSTPROCESSINGPASS_H
