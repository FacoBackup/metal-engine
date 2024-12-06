#ifndef METAL_ENGINE_DOCKSERVICE_H
#define METAL_ENGINE_DOCKSERVICE_H

#include "../../common/runtime/AbstractRuntimeComponent.h"
#include "imgui.h"
#include "../repository/dock/DockDTO.h"
#include "../ui/docks/DockSpacePanel.h"

namespace Metal {
    class AbstractPanel;

    class DockService final : public AbstractRuntimeComponent {
    public:
        void buildViews(ImGuiID windowId, AbstractPanel *panel) const;

        static void createDockSpace(DockDTO *dockSpace, ImGuiID *dockMainId);

        static void addWindow(DockDTO *d, AbstractPanel *panel);

        void prepareForRemoval(DockDTO *dock, DockSpacePanel *dockSpacePanel) const;

        void updateForRemoval(AbstractPanel *panel) const;

        explicit DockService(ApplicationContext &context);
    };
} // Metal

#endif
