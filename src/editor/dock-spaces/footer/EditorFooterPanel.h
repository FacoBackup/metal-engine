#ifndef METAL_ENGINE_EDITORFOOTERPANEL_H
#define METAL_ENGINE_EDITORFOOTERPANEL_H

#include "../../abstract/AbstractPanel.h"

namespace Metal {
    struct EditorRepository;

    class AsyncTaskPanel;

    class EditorFooterPanel final : public AbstractPanel {
        EditorRepository *editorRepository = nullptr;
        AsyncTaskPanel *asyncTaskPanel = nullptr;

        void renderShortcuts();
        static void framerate();

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"EditorRepository", &editorRepository}
            };
        }

        void onInitialize() override;

        void onSync() override;
    };
}

#endif
