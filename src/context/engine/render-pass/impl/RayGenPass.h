#ifndef RAYGENPASS_H
#define RAYGENPASS_H

#include "../AbstractRenderPass.h"

namespace Metal {
    class RayGenPass final : public AbstractRenderPass {
    public:
        explicit RayGenPass(ApplicationContext &context)
            : AbstractRenderPass(context) {
        }

        void onSync() override;

        PipelineInstance *getPipeline() override;
    };
} // Metal

#endif //RAYGENPASS_H
