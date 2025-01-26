#ifndef GBUFFERSHADINGPASS_H
#define GBUFFERSHADINGPASS_H
#include "../AbstractComputePass.h"

namespace Metal {
    class PathTracerPass final : public AbstractComputePass {
        bool isFirstRun = true;

    public:
        explicit PathTracerPass(ApplicationContext &context)
            : AbstractComputePass(context) {
        }

        void onSync() override;

        void onInitialize() override;
    };
} // Metal

#endif //GBUFFERSHADINGPASS_H
