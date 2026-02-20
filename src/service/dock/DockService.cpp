#include <imgui_internal.h>
#include <algorithm>
#include "DockService.h"
#include "../../repository/dock/DockDTO.h"
#include "../../context/editor/abstract/AbstractPanel.h"
#include "../../context/editor/dock-spaces/docks/AbstractDockPanel.h"
#include "../../context/editor/dock-spaces/docks/DockSpacePanel.h"
#include "../../context/ApplicationContext.h"

namespace Metal {
    DockService::DockService() : AbstractRuntimeComponent() {
    }

    void DockService::buildViews(ImGuiID windowId, AbstractPanel *panel) const {
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable) {
            DockRepository &dockRepository = CTX.dockRepository;
            if (dockRepository.isInitialized) {
                return;
            }
            dockRepository.isInitialized = true;

            std::vector<IPanel *> toKeep{};
            for (auto *child: panel->getChildren()) {
                auto *p = dynamic_cast<DockSpacePanel *>(child);
                if (p == nullptr) {
                    toKeep.push_back(child);
                } else {
                    p->removeAllChildren();
                    delete p->getDock();
                    delete p;
                }
            }

            panel->getChildren().clear();
            for (auto *child: toKeep) {
                panel->getChildren().push_back(child);
            }

            ImGui::DockBuilderRemoveNode(windowId);
            ImGui::DockBuilderAddNode(windowId, ImGuiDockNodeFlags_NoTabBar);
            ImGui::DockBuilderSetNodeSize(windowId, ImGui::GetMainViewport()->Size);
            {
                dockRepository.top.direction = TOP;
                dockRepository.top.origin = nullptr;
                dockRepository.top.outAtOppositeDir = nullptr;
                dockRepository.top.splitDir = ImGuiDir_Right;

                createDockSpace(&dockRepository.top, &windowId);
                addWindow(&dockRepository.top, panel);
            }

            {
                dockRepository.rightTop.origin = &dockRepository.top;
                dockRepository.rightTop.outAtOppositeDir = &dockRepository.top;
                dockRepository.rightTop.splitDir = ImGuiDir_Down;
                dockRepository.rightTop.direction = RIGHT_TOP;
                createDockSpace(&dockRepository.rightTop, &windowId);
                addWindow(&dockRepository.rightTop, panel);
            }

            {
                dockRepository.rightBottom.origin = &dockRepository.rightTop;
                dockRepository.rightBottom.outAtOppositeDir = &dockRepository.rightTop;

                dockRepository.rightBottom.splitDir = ImGuiDir_Down;
                dockRepository.rightBottom.direction = RIGHT_BOTTOM;
                createDockSpace(&dockRepository.rightBottom, &windowId);
                addWindow(&dockRepository.rightBottom, panel);
            }

            {
                dockRepository.bottom.origin = nullptr;
                dockRepository.bottom.outAtOppositeDir = nullptr;
                dockRepository.bottom.splitDir = ImGuiDir_Down;

                dockRepository.bottom.direction = BOTTOM;
                createDockSpace(&dockRepository.bottom, &windowId);
                addWindow(&dockRepository.bottom, panel);
            }
            ImGui::DockBuilderDockWindow(dockRepository.top.internalId.c_str(), windowId);
            ImGui::DockBuilderFinish(windowId);
        }
    }

    void DockService::createDockSpace(DockDTO *dockSpace, ImGuiID *dockMainId) {
        ImGuiID origin = *dockMainId;
        if (dockSpace->origin != nullptr) {
            origin = dockSpace->origin->nodeId;
        }

        ImGuiID *target = dockMainId;
        if (dockSpace->outAtOppositeDir != nullptr) {
            target = &dockSpace->outAtOppositeDir->nodeId;
        }
        dockSpace->nodeId = ImGui::DockBuilderSplitNode(origin, dockSpace->splitDir,
                                                        dockSpace->sizeRatioForNodeAtDir,
                                                        nullptr, target);
        ImGuiDockNode *imGuiDockNode = ImGui::DockBuilderGetNode(dockSpace->nodeId);
        imGuiDockNode->LocalFlags = ImGuiDockNodeFlags_NoTabBar;
    }

    void DockService::addWindow(DockDTO *d, AbstractPanel *panel) {
        ImGui::DockBuilderDockWindow(d->internalId.c_str(), d->nodeId);
        for (auto *l: panel->getChildren()) {
            if (dynamic_cast<DockSpacePanel *>(l)) {
                panel->appendChild(new DockSpacePanel(dynamic_cast<DockSpacePanel *>(l), d));
                return;
            }
        }
        panel->appendChild(new DockSpacePanel(nullptr, d));
    }
}
