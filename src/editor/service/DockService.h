#ifndef METAL_ENGINE_DOCKSERVICE_H
#define METAL_ENGINE_DOCKSERVICE_H

#include "imgui.h"
#include "../dto/DockDTO.h"
#include "../dock-spaces/docks/DockSpacePanel.h"
#include "../../common/IService.h"

namespace Metal {
    class AbstractPanel;

    class DockService final : public IService, public IInit {
        DockDTO center{&DockSpace::VIEWPORT};
        std::vector<DockDTO *> bottom;
        std::vector<DockDTO *> left;
        std::vector<DockDTO *> right;

        bool isInitialized = false;

    public:
        void onInitialize() override;

        void buildViews(ImGuiID windowId, AbstractPanel *panel);

        static void createDockSpace(DockDTO *dockSpace, ImGuiID *dockMainId);

        static void addWindow(DockDTO *d, AbstractPanel *panel);
    };
} // Metal

#endif
