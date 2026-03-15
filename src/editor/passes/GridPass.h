#ifndef GRIDRENDERPASS_H
#define GRIDRENDERPASS_H
#include "../../engine/passes/AbstractRenderPass.h"
#include "../dto/GridPushConstant.h"

namespace Metal {
    class PipelineService;
    struct EditorRepository;

    class GridPass final : public AbstractRenderPass {
        GridPushConstant pushConstant{};

        PipelineService *pipelineService = nullptr;
        EditorRepository *editorRepository = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"PipelineService", &pipelineService},
                {"EditorRepository", &editorRepository}
            };
        }

        void onInitialize() override;

        bool shouldRun() override;

        void onSync() override;
    };
} // Metal

#endif //GRIDRENDERPASS_H
