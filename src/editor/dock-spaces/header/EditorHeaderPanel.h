#ifndef METAL_ENGINE_EDITORHEADERPANEL_H
#define METAL_ENGINE_EDITORHEADERPANEL_H

#include "../../abstract/AbstractPanel.h"

namespace Metal {
    class GLFWContext;
    class EngineContext;
    struct DirectoryService;

    class EditorHeaderPanel final : public AbstractPanel {
        AbstractPanel *asyncTask = nullptr;

        GLFWContext *glfwContext = nullptr;
        EngineContext *engineContext = nullptr;
        DirectoryService *directoryService = nullptr;

        void renderFileTab();

        void renderWindowControls();

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"GLFWContext", glfwContext},
                {"EngineContext", engineContext},
                {"DirectoryService", directoryService}
            };
        }

        void onInitialize() override;

        void onSync() override;
    };
} // Metal

#endif
