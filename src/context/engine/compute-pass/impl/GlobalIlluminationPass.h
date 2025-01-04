#ifndef GLOBALILLUMINATIONPASS_H
#define GLOBALILLUMINATIONPASS_H
#include "../AbstractComputePass.h"

namespace Metal {
    class GlobalIlluminationPass final : public AbstractComputePass {
    public:
        explicit GlobalIlluminationPass(ApplicationContext &context)
            : AbstractComputePass(context) {
        }

        void onSync() override;

        bool shouldRun() override;

        PipelineInstance *getPipeline() override;
    };
} // Metal

#endif //GLOBALILLUMINATIONPASS_H
