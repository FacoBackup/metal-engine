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
    struct DirectoryService;

    class ViewportPanel final : public AbstractDockPanel {
        bool isFirstMovement = false;
        AbstractPanel *headerPanel = nullptr;
        AbstractPanel *gizmoPanel = nullptr;
        AbstractPanel *cameraPanel = nullptr;
        AbstractPanel *engineFramePanel = nullptr;

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
                {"CameraService", cameraService},
                {"SelectionService", selectionService},
                {"EditorRepository", editorRepository},
                {"WorldRepository", worldRepository},
                {"RuntimeRepository", runtimeRepository},
                {"EngineContext", engineContext},
                {"DirectoryService", directoryService}
            };
        }

        void onInitialize() override;

        void onSync() override;

        void updateCamera();

        void updateInputs() const;
    };
}

#endif
