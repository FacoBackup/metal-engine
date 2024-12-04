#include <imgui_internal.h>
#include "DockService.h"
#include "../repository/dock/DockDTO.h"

namespace Metal {
    DockService::DockService(ApplicationContext &context) : AbstractRuntimeComponent(context) {
    }

    void DockService::buildViews(ImGuiID windowId, Metal::AbstractPanel *panel) {
        if (isInitialized) {
            return;
        }
        isInitialized = true;
        ImGui::DockBuilderRemoveNode(windowId);
        ImGui::DockBuilderAddNode(windowId, NO_TAB_BAR_FLAG);
        ImGui::DockBuilderSetNodeSize(windowId, ImGui::GetMainViewport()->Size);

        context.getEditorContext().dockRepository.center.setDirection(DockPosition.CENTER);
        context.getEditorContext().dockRepository.center.setOrigin(null);
        context.getEditorContext().dockRepository.center.setOutAtOppositeDir(null);
        context.getEditorContext().dockRepository.center.setSplitDir(ImGuiDir_Right);

        createDockSpace(dockRepository.center, windowId);

        addWindow(dockRepository.center, panel);
        std::vector<DockDTO> left = dockRepository.left;
        for (int i = 0; i < left.size(); i++) {
            DockDTO dockSpace = left.get(i);
            if (i == 0) {
                dockSpace.setOrigin(dockRepository.center);
                dockSpace.setOutAtOppositeDir(dockRepository.center);
            } else {
                DockDTO previous = left.get(i - 1);
                dockSpace.setOrigin(previous);
                dockSpace.setOutAtOppositeDir(previous);
            }
            dockSpace.setSplitDir(ImGuiDir.Down);
            dockSpace.setDirection(DockPosition.LEFT);
            createDockSpace(dockSpace, windowId);
            addWindow(dockSpace, panel);
        }

        List <DockDTO> right = dockRepository.right;
        for (int i = 0; i < right.size(); i++) {
            DockDTO dockSpace = right.get(i);
            if (i == 0) {
                dockSpace.setOrigin(dockRepository.center);
                dockSpace.setOutAtOppositeDir(dockRepository.center);
            } else {
                DockDTO previous = right.get(i - 1);
                dockSpace.setOrigin(previous);
                dockSpace.setOutAtOppositeDir(previous);
            }
            dockSpace.setSplitDir(ImGuiDir_Down);
            dockSpace.setDirection(DockPosition.RIGHT);
            createDockSpace(dockSpace, windowId);
            addWindow(dockSpace, panel);
        }

        List <DockDTO> bottom = dockRepository.bottom;
        for (int i = 0, bottomSize = bottom.size(); i < bottomSize; i++) {
            DockDTO dockSpace = bottom.get(i);
            if (i == 0) {
                dockSpace.setOrigin(null);
                dockSpace.setOutAtOppositeDir(null);
                dockSpace.setSplitDir(ImGuiDir_Down);
            } else {
                DockDTO previous = bottom.get(i - 1);
                dockSpace.setOrigin(previous);
                dockSpace.setOutAtOppositeDir(previous);
                dockSpace.setSplitDir(ImGuiDir_Right);
            }
            dockSpace.setDirection(DockPosition.BOTTOM);
            createDockSpace(dockSpace, windowId);
            addWindow(dockSpace, panel);
        }

        ImGui::DockBuilderDockWindow(dockRepository.center.getInternalId(), windowId);
        ImGui::DockBuilderFinish(windowId); 
    }
} 