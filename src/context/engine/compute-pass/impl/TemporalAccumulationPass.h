#ifndef TEMPORALACCUMULATIONPASS_H
#define TEMPORALACCUMULATIONPASS_H
#include "../AbstractComputePass.h"

namespace Metal {
    class TemporalAccumulationPass final : public AbstractComputePass {
    public:
        explicit TemporalAccumulationPass(const std::string &id) : AbstractComputePass(id) {
        }

        void onSync() override;

        void onInitialize() override;
    };
} // Metal

#endif //TEMPORALACCUMULATIONPASS_H
