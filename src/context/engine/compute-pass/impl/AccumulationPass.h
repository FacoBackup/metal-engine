#ifndef ACCUMULATIONPASS_H
#define ACCUMULATIONPASS_H
#include "../AbstractComputePass.h"

namespace Metal {
    class AccumulationPass final : public AbstractComputePass {
    public:
        explicit AccumulationPass(ApplicationContext &context)
            : AbstractComputePass(context) {
        }

        void onSync() override;

        void onInitialize() override;

        bool shouldRun() override;
    };
} // Metal

#endif //ACCUMULATIONPASS_H
