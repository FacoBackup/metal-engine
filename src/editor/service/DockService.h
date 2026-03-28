#ifndef METAL_ENGINE_DOCKSERVICE_H
#define METAL_ENGINE_DOCKSERVICE_H

#include "imgui.h"
#include "../dto/DockDefinition.h"
#include "../ui/docks/DockSpacePanel.h"
#include "common/IService.h"

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

        [[nodiscard]] std::shared_ptr<DockDefinition> getCenter() const { return dockRepository->getCenter(); }

        [[nodiscard]] const std::vector<std::shared_ptr<DockDefinition> > &getBottom() const {
            return dockRepository->getBottom();
        }

        [[nodiscard]] const std::vector<std::shared_ptr<DockDefinition> > &getLeft() const {
            return dockRepository->getLeft();
        }

        [[nodiscard]] const std::vector<std::shared_ptr<DockDefinition> > &getRight() const {
            return dockRepository->getRight();
        }

        [[nodiscard]] bool getIsInitialized() const { return isInitialized; }
        void setIsInitialized(bool value) { isInitialized = value; }

        void onInitialize() override;

        void buildViews(ImGuiID windowId, AbstractPanel *panel);

        void removeDock(std::shared_ptr<DockDefinition> dock);

        void addLeftDock();

        void addBottomDock();

        void addRightDock();

        static void createDockSpace(std::shared_ptr<DockDefinition> dockSpace, ImGuiID *dockMainId);

        static void addWindow(std::shared_ptr<DockDefinition> d, AbstractPanel *panel);
    };
} // Metal

#endif
