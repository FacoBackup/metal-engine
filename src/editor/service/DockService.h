#ifndef METAL_ENGINE_DOCKSERVICE_H
#define METAL_ENGINE_DOCKSERVICE_H

#include "imgui.h"
#include "../dto/DockDTO.h"
#include "../dock-spaces/docks/DockSpacePanel.h"
#include "../../common/IService.h"

namespace Metal {
    class AbstractPanel;

    class DockService final : public IService, public IInit {
        std::shared_ptr<DockDTO> center = std::make_shared<DockDTO>(&DockSpace::VIEWPORT);
        std::vector<std::shared_ptr<DockDTO> > bottom;
        std::vector<std::shared_ptr<DockDTO> > left;
        std::vector<std::shared_ptr<DockDTO> > right;

        bool isInitialized = false;

    public:
        [[nodiscard]] std::shared_ptr<DockDTO> getCenter() const { return center; }
        [[nodiscard]] const std::vector<std::shared_ptr<DockDTO> > &getBottom() const { return bottom; }
        [[nodiscard]] const std::vector<std::shared_ptr<DockDTO> > &getLeft() const { return left; }
        [[nodiscard]] const std::vector<std::shared_ptr<DockDTO> > &getRight() const { return right; }
        [[nodiscard]] bool getIsInitialized() const { return isInitialized; }
        void setIsInitialized(bool value) { isInitialized = value; }

        void onInitialize() override;

        void buildViews(ImGuiID windowId, AbstractPanel *panel);

        void removeDock(std::shared_ptr<DockDTO> dock);
        
        void addLeftDock();
        void addBottomDock();
        void addRightDock();

        static void createDockSpace(std::shared_ptr<DockDTO> dockSpace, ImGuiID *dockMainId);

        static void addWindow(std::shared_ptr<DockDTO> d, AbstractPanel *panel);
    };
} // Metal

#endif
