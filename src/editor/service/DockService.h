#ifndef METAL_ENGINE_DOCKSERVICE_H
#define METAL_ENGINE_DOCKSERVICE_H

#include "imgui.h"
#include "../dto/DockDTO.h"
#include "../dock-spaces/docks/DockSpacePanel.h"
#include "../../common/IService.h"

#include "../repository/DockRepository.h"

namespace Metal {
    class AbstractPanel;

    class DockService final : public IService, public IInit {
        DockRepository *dockRepository = nullptr;

        bool isInitialized = false;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"DockRepository", &dockRepository}
            };
        }

        [[nodiscard]] std::shared_ptr<DockDTO> getCenter() const { return dockRepository->center; }
        [[nodiscard]] const std::vector<std::shared_ptr<DockDTO> > &getBottom() const { return dockRepository->bottom; }
        [[nodiscard]] const std::vector<std::shared_ptr<DockDTO> > &getLeft() const { return dockRepository->left; }
        [[nodiscard]] const std::vector<std::shared_ptr<DockDTO> > &getRight() const { return dockRepository->right; }
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
