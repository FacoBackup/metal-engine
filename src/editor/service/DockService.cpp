#include <imgui_internal.h>
#include <algorithm>
#include "../../../service/dock/DockService.h"
#include "../dto/DockDTO.h"
#include "../abstract/AbstractPanel.h"
#include "../dock-spaces/docks/AbstractDockPanel.h"
#include "../dock-spaces/docks/DockSpacePanel.h"
#include "../../ApplicationContext.h"

namespace Metal {
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

            const auto &left = dockRepository.left;
            for (size_t i = 0; i < left.size(); i++) {
                DockDTO *dockSpace = left[i];
                if (i == 0) {
                    dockSpace->origin = nullptr;
                    dockSpace->outAtOppositeDir = nullptr;
                    dockSpace->splitDir = ImGuiDir_Left;
                } else {
                    DockDTO *previous = left[i - 1];
                    dockSpace->origin = previous;
                    dockSpace->outAtOppositeDir = previous;
                    dockSpace->splitDir = ImGuiDir_Down;
                }
                createDockSpace(dockSpace, &windowId);
                addWindow(dockSpace, panel);
            }

            const auto &right = dockRepository.right;
            for (size_t i = 0; i < right.size(); i++) {
                DockDTO *dockSpace = right[i];
                if (i == 0) {
                    dockSpace->origin = nullptr;
                    dockSpace->outAtOppositeDir = nullptr;
                    dockSpace->splitDir = ImGuiDir_Right;
                } else {
                    DockDTO *previous = right[i - 1];
                    dockSpace->origin = previous;
                    dockSpace->outAtOppositeDir = previous;
                    dockSpace->splitDir = ImGuiDir_Down;
                }
                createDockSpace(dockSpace, &windowId);
                addWindow(dockSpace, panel);
            }

            const auto &bottom = dockRepository.bottom;
            for (size_t i = 0, bottomSize = bottom.size(); i < bottomSize; i++) {
                DockDTO *dockSpace = bottom[i];
                if (i == 0) {
                    dockSpace->origin = nullptr;
                    dockSpace->outAtOppositeDir = nullptr;
                    dockSpace->splitDir = ImGuiDir_Down;
                } else {
                    DockDTO *previous = bottom[i - 1];
                    dockSpace->origin = previous;
                    dockSpace->outAtOppositeDir = previous;
                    dockSpace->splitDir = ImGuiDir_Right;
                }
                createDockSpace(dockSpace, &windowId);
                addWindow(dockSpace, panel);
            }

            dockRepository.center.nodeId = windowId;
            addWindow(&dockRepository.center, panel);

            ImGui::DockBuilderDockWindow(dockRepository.center.internalId.c_str(), windowId);
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
