#ifndef ACCUMULATIONPASS_H
#define ACCUMULATIONPASS_H
#include "../AbstractComputePass.h"

namespace Metal {
    class AccumulationPass final : public AbstractComputePass {
    public:
        explicit AccumulationPass(const std::string &id) : AbstractComputePass(id) {}

        void onSync() override;

        void onInitialize() override;

    };
} // Metal

#endif //ACCUMULATIONPASS_H
