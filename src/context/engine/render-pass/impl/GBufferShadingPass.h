#ifndef GBUFFERSHADINGPASS_H
#define GBUFFERSHADINGPASS_H
#include "../AbstractRenderPass.h"

namespace Metal {

    class GBufferShadingPass final : public AbstractRenderPass {
    public:
        explicit GBufferShadingPass(ApplicationContext &context)
            : AbstractRenderPass(context) {
        }

        void onSync() override;

        PipelineInstance *getPipeline() override;
    };

} // Metal

#endif //GBUFFERSHADINGPASS_H
