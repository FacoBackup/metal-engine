#ifndef ATMOSPHEREPASS_H
#define ATMOSPHEREPASS_H
#include "../AbstractRenderPass.h"

namespace Metal {
    class AtmospherePass final : public AbstractRenderPass {
    public:
        explicit AtmospherePass(ApplicationContext &context)
            : AbstractRenderPass(context) {
        }

        bool shouldRun() override;

        void onSync() override;

        PipelineInstance *getPipeline() override;
    };
} // Metal

#endif //ATMOSPHEREPASS_H
