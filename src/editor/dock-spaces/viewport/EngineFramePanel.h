#ifndef ENGINE_FRAME_PANEL_H
#define ENGINE_FRAME_PANEL_H

#include "../../abstract/AbstractPanel.h"
#include <memory>
#include <imgui.h>

namespace Metal {
    class EngineFrame;
    class VulkanContext;
    struct EngineRepository;
    class EngineContext;
    class DescriptorSetService;
    class PickingService;
    class SelectionService;

    class EngineFramePanel final : public AbstractPanel {
        VulkanContext *vulkanContext = nullptr;
        EngineRepository *engineRepository = nullptr;
        EngineContext *engineContext = nullptr;
        DescriptorSetService *descriptorSetService = nullptr;
        PickingService *pickingService = nullptr;
        SelectionService *selectionService = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"VulkanContext", &vulkanContext},
                {"EngineRepository", &engineRepository},
                {"EngineContext", &engineContext},
                {"DescriptorSetService", &descriptorSetService},
                {"PickingService", &pickingService},
                {"SelectionService", &selectionService}
            };
        }

        void onInitialize() override;

        void onSync() override;

        void handleViewportPicking(const ImVec2 &imageMin, const ImVec2 &imageMax) const;
    };
} // Metal

#endif //ENGINE_FRAME_PANEL_H
