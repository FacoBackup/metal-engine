#ifndef ENGINE_FRAME_PANEL_H
#define ENGINE_FRAME_PANEL_H

#include "editor/ui/abstract/AbstractPanel.h"
#include <memory>
#include <imgui.h>

namespace Metal {
    class CameraService;
    class EngineFrame;
    class VulkanContext;
    struct EngineRepository;
    struct EditorRepository;
    class EngineContext;
    class DescriptorSetService;
    class PickingService;
    class SelectionService;
    struct WorldRepository;
    struct EditorCameraRepository;
    class EditorCameraService;

    class EngineFramePanel final : public AbstractPanel {
        VulkanContext *vulkanContext = nullptr;
        EngineRepository *engineRepository = nullptr;
        EditorRepository *editorRepository = nullptr;
        EngineContext *engineContext = nullptr;
        DescriptorSetService *descriptorSetService = nullptr;
        PickingService *pickingService = nullptr;
        SelectionService *selectionService = nullptr;
        WorldRepository *worldRepository = nullptr;
        CameraService *cameraService = nullptr;
        EditorCameraService *editorCameraService = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"VulkanContext", &vulkanContext},
                {"EngineRepository", &engineRepository},
                {"EditorRepository", &editorRepository},
                {"EngineContext", &engineContext},
                {"cameraService", &cameraService},
                {"DescriptorSetService", &descriptorSetService},
                {"PickingService", &pickingService},
                {"SelectionService", &selectionService},
                {"WorldRepository", &worldRepository},
                {"EditorCameraService", &editorCameraService}
            };
        }

        void onInitialize() override;

        void onSync() override;

        void handleViewportPicking(const ImVec2 &imageMin, const ImVec2 &imageMax) const;
    };
} // Metal

#endif //ENGINE_FRAME_PANEL_H
