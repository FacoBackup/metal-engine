#ifndef SURFACECACHERESETPASS_H
#define SURFACECACHERESETPASS_H

#include "../AbstractComputePass.h"

namespace Metal {
    class SurfaceCacheResetPass final : public AbstractComputePass {
    public:
        explicit SurfaceCacheResetPass(ApplicationContext &context)
            : AbstractComputePass(context) {
        }

        void onSync() override;

        void onInitialize() override;

        bool shouldRun() override;
    };
} // Metal

#endif //SURFACECACHERESETPASS_H
