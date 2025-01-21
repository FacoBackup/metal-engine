#ifndef ICONRENDERPASS_H
#define ICONRENDERPASS_H
#include "../../AbstractRenderPass.h"
#include "../../../../../dto/push-constant/IconPushConstant.h"

namespace Metal {
    class IconsPass final : public AbstractRenderPass {
        IconPushConstant pushConstant{};

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
