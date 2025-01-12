#ifndef GLOBALILLUMINATIONPASS_H
#define GLOBALILLUMINATIONPASS_H
#include "../AbstractComputePass.h"

namespace Metal {
    class GlobalIlluminationPass final : public AbstractComputePass {
        bool isFirstRun = true;
    public:
        explicit GlobalIlluminationPass(ApplicationContext &context)
            : AbstractComputePass(context) {
        }

        void onSync() override;

        PipelineInstance *getPipeline() override;

        void clearTexture(const VkImage &image) const;

        void changeTextureModeEnd() const;

        void changeTextureModeBegin() const;
    };
} // Metal

#endif //GLOBALILLUMINATIONPASS_H
