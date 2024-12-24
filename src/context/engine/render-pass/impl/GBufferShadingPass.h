#ifndef GBUFFERSHADINGPASS_H
#define GBUFFERSHADINGPASS_H
#include "../AbstractRenderPass.h"
#include "../../../../dto/push-constant/GBufferShadingPushConstant.h"

namespace Metal {

    class GBufferShadingPass final : public AbstractRenderPass {
        GBufferShadingPushConstant pushConstant{};
    public:
        explicit GBufferShadingPass(ApplicationContext &context)
            : AbstractRenderPass(context) {
        }

        void onSync() override;

        PipelineInstance *getPipeline() override;
    };

} // Metal

#endif //GBUFFERSHADINGPASS_H
