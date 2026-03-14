#ifndef METAL_ENGINE_EDITORFOOTERPANEL_H
#define METAL_ENGINE_EDITORFOOTERPANEL_H

#include "../../abstract/AbstractPanel.h"

namespace Metal {
    struct EditorRepository;

    class EditorFooterPanel final : public AbstractPanel {
        EditorRepository *editorRepository = nullptr;

        void renderShortcuts();
        static void framerate();

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"EditorRepository", editorRepository}
            };
        }

        void onSync() override;
    };
}

#endif
