#ifndef ICONRENDERPASS_H
#define ICONRENDERPASS_H
#include "../../engine/passes/AbstractRenderPass.h"

namespace Metal {
    class IconsPass final : public AbstractRenderPass {
    public:
        explicit IconsPass(const std::string &id) : AbstractRenderPass(id) {}

        void onInitialize() override;

        bool shouldRun() override;

        void onSync() override;
    };
} // Metal

#endif //ICONRENDERPASS_H
