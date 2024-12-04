#ifndef METAL_ENGINE_DOCKSERVICE_H
#define METAL_ENGINE_DOCKSERVICE_H

#include "../../common/runtime/AbstractRuntimeComponent.h"
#include "imgui.h"
#include "../repository/dock/DockDTO.h"
#include "../ui/docks/DockSpacePanel.h"

namespace Metal {
    class AbstractPanel;

    class DockService : public AbstractRuntimeComponent {
        static const int NO_TAB_BAR_FLAG = 1 << 12;

        bool isInitialized;
    public:
        void buildViews(ImGuiID &windowId, AbstractPanel *panel);

        void createDockSpace(DockDTO &dockSpace, ImGuiID &dockMainId);

        void addWindow(DockDTO &d, AbstractPanel *panel);

        void prepareForRemoval(DockDTO &dock, DockSpacePanel *dockSpacePanel);

        void updateForRemoval(AbstractPanel *panel);

        explicit DockService(ApplicationContext &context);
    };

} // Metal

#endif
