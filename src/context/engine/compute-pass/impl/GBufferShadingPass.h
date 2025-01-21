#ifndef GBUFFERSHADINGPASS_H
#define GBUFFERSHADINGPASS_H
#include "../../render-pass/AbstractRenderPass.h"
#include "../AbstractComputePass.h"

namespace Metal {

    class GBufferShadingPass final : public AbstractComputePass {
        bool isFirstRun = true;
    public:
        explicit GBufferShadingPass(ApplicationContext &context)
            : AbstractComputePass(context) {
        }

        void onSync() override;

        void onInitialize() override;
    };

} // Metal

#endif //GBUFFERSHADINGPASS_H
