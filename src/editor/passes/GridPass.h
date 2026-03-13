#ifndef GRIDRENDERPASS_H
#define GRIDRENDERPASS_H
#include "../../engine/passes/AbstractRenderPass.h"
#include "../dto/GridPushConstant.h"

namespace Metal {
    class GridPass final : public AbstractRenderPass {
        GridPushConstant pushConstant{};

    public:
        explicit GridPass(const std::string &id) : AbstractRenderPass(id) {}

        void onInitialize() override;

        bool shouldRun() override;

        void onSync() override;
    };
} // Metal

#endif //GRIDRENDERPASS_H
