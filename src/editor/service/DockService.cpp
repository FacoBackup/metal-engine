#include <imgui_internal.h>
#include <algorithm>
#include "DockService.h"
#include "../dto/DockDTO.h"
#include "../abstract/AbstractPanel.h"
#include "../dock-spaces/docks/AbstractDockPanel.h"
#include "../dock-spaces/docks/DockSpacePanel.h"
#include "../../ApplicationContext.h"

namespace Metal {
    void DockService::onInitialize() {
        auto *rightT = new DockDTO{&DockSpace::WORLD};
        auto *leftT = new DockDTO{&DockSpace::REPOSITORIES};
        auto *leftB = new DockDTO{&DockSpace::INSPECTOR};
        auto *rightB = new DockDTO{&DockSpace::CONSOLE};
        auto *b = new DockDTO{&DockSpace::FILES};

        center.isCenter = true;
        center.sizeRatioForNodeAtDir = 0.5f;
        rightT->sizeRatioForNodeAtDir = 0.25f;
        leftT->sizeRatioForNodeAtDir = 0.2f;
        leftB->sizeRatioForNodeAtDir = 0.5f;
        rightB->sizeRatioForNodeAtDir = 0.25f;
        b->sizeRatioForNodeAtDir = 0.25f;

        right.push_back(rightT);
        right.push_back(rightB);

        left.push_back(leftT);
        left.push_back(leftB);

        bottom.push_back(b);
    }

    void DockService::buildViews(ImGuiID windowId, AbstractPanel *panel) {
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable) {
            if (isInitialized) {
                return;
            }
            isInitialized = true;

            std::vector<AbstractPanel *> toKeep{};
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

            center.nodeId = windowId;
            addWindow(&center, panel);

            ImGui::DockBuilderDockWindow(center.internalId.c_str(), windowId);
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
                panel->initializePanel(new DockSpacePanel(dynamic_cast<DockSpacePanel *>(l), d));
                return;
            }
        }
        panel->initializePanel(new DockSpacePanel(nullptr, d));
    }
}
