#ifndef RTPASS_H
#define RTPASS_H
#include "../AbstractComputePass.h"

namespace Metal {
    class RTPass final : public AbstractComputePass {
        bool isFirstRun = true;

    public:
        void onSync() override;

        void onInitialize() override;
    };
} // Metal

#endif //RTPASS_H
