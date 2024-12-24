#ifndef METAL_ENGINE_DOCKSERVICE_H
#define METAL_ENGINE_DOCKSERVICE_H

#include "imgui.h"
#include "../../repository/dock/DockDTO.h"
#include "../../context/editor/dock-spaces/docks/DockSpacePanel.h"
#include "../../common/AbstractRuntimeComponent.h"

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
