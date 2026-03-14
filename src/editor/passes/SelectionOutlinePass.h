#ifndef SELECTEDDOTPASS_H
#define SELECTEDDOTPASS_H

#include "../../engine/passes/AbstractRenderPass.h"
#include "../dto/SelectedDotPushConstant.h"

namespace Metal {
    class PipelineService;
    struct EditorRepository;

    class SelectionOutlinePass final : public AbstractRenderPass {
        SelectedDotPushConstant pushConstant{};

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

#endif // SELECTEDDOTPASS_H
