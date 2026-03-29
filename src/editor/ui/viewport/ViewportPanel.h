#ifndef METAL_ENGINE_VIEWPORTPANEL_H
#define METAL_ENGINE_VIEWPORTPANEL_H

#include "../docks/AbstractDockPanel.h"

namespace Metal {
    class CameraService;
    class SelectionService;
    struct EditorRepository;
    struct WorldRepository;
    struct RuntimeRepository;
    class EngineContext;
    class DirectoryService;

    class ViewportPanel final : public AbstractDockPanel {
        bool isFirstMovement = false;
        float lastMouseX = 0;
        float lastMouseY = 0;
        std::shared_ptr<AbstractPanel> headerPanel = nullptr;
        std::shared_ptr<AbstractPanel> gizmoPanel = nullptr;
        std::shared_ptr<AbstractPanel> cameraGizmoPanel = nullptr;
        std::shared_ptr<AbstractPanel> engineFramePanel = nullptr;

        CameraService *cameraService = nullptr;
        SelectionService *selectionService = nullptr;
        EditorRepository *editorRepository = nullptr;
        WorldRepository *worldRepository = nullptr;
        RuntimeRepository *runtimeRepository = nullptr;
        EngineContext *engineContext = nullptr;
        DirectoryService *directoryService = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"CameraService", &cameraService},
                {"SelectionService", &selectionService},
                {"EditorRepository", &editorRepository},
                {"WorldRepository", &worldRepository},
                {"RuntimeRepository", &runtimeRepository},
                {"EngineContext", &engineContext},
                {"DirectoryService", &directoryService}
            };
        }

        void onInitialize() override;

        void onSync() override;

        void updateCamera();

        void updateInputs() const;
    };
}

#endif
