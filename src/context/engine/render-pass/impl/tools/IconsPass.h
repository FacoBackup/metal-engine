#ifndef ICONRENDERPASS_H
#define ICONRENDERPASS_H
#include "../../AbstractRenderPass.h"

namespace Metal {
    class IconsPass final : public AbstractRenderPass {
    public:
        explicit IconsPass(ApplicationContext &context)
            : AbstractRenderPass(context) {
        }

        void onInitialize() override;

        bool shouldRun() override;

        void onSync() override;
    };
} // Metal

#endif //ICONRENDERPASS_H
