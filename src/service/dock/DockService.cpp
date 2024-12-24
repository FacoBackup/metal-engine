#include <imgui_internal.h>
#include "DockService.h"
#include "../../repository/dock/DockDTO.h"
#include "../../context/editor/abstract/AbstractPanel.h"
#include "../../context/editor/dock-spaces/docks/AbstractDockPanel.h"
#include "../../context/editor/dock-spaces/docks/DockSpacePanel.h"
#include "../../context/ApplicationContext.h"

namespace Metal {
    DockService::DockService(ApplicationContext &context) : AbstractRuntimeComponent(context) {
    }

    void DockService::buildViews(ImGuiID windowId, AbstractPanel *panel) const {
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable) {
            DockRepository &dockRepository = context.dockRepository;
            if (dockRepository.isInitialized) {
                return;
            }
            dockRepository.isInitialized = true;

            std::vector<IPanel *> toKeep{};
            for (auto *child: panel->getChildren()) {
                if (!dynamic_cast<DockSpacePanel *>(child)) {
                    toKeep.push_back(child);
                } else {
                    auto *p = dynamic_cast<DockSpacePanel *>(child);
                    p->removeAllChildren();
                    delete p->getDock();
                    delete child;
                }
            }

            panel->getChildren().clear();
            for (auto *child: toKeep) {
                panel->getChildren().push_back(child);
            }

            ImGui::DockBuilderRemoveNode(windowId);
            ImGui::DockBuilderAddNode(windowId, ImGuiDockNodeFlags_NoTabBar);
            ImGui::DockBuilderSetNodeSize(windowId, ImGui::GetMainViewport()->Size);

            dockRepository.center.direction = CENTER;
            dockRepository.center.origin = nullptr;
            dockRepository.center.outAtOppositeDir = nullptr;
            dockRepository.center.splitDir = ImGuiDir_Right;

            createDockSpace(&dockRepository.center, &windowId);
            addWindow(&dockRepository.center, panel);

            const auto &left = dockRepository.left;
            for (size_t i = 0; i < left.size(); i++) {
                DockDTO *dockSpace = left[i];
                if (i == 0) {
                    dockSpace->origin = &dockRepository.center;
                    dockSpace->outAtOppositeDir = &dockRepository.center;
                } else {
                    DockDTO *previous = left[i - 1];
                    dockSpace->origin = previous;
                    dockSpace->outAtOppositeDir = previous;
                }
                dockSpace->splitDir = ImGuiDir_Down;
                dockSpace->direction = LEFT;
                createDockSpace(dockSpace, &windowId);
                addWindow(dockSpace, panel);
            }

            const auto &right = dockRepository.right;
            for (size_t i = 0; i < right.size(); i++) {
                DockDTO *dockSpace = right[i];
                if (i == 0) {
                    dockSpace->origin = &dockRepository.center;
                    dockSpace->outAtOppositeDir = &dockRepository.center;
                } else {
                    DockDTO *previous = right[i - 1];
                    dockSpace->origin = previous;
                    dockSpace->outAtOppositeDir = previous;
                }
                dockSpace->splitDir = ImGuiDir_Down;
                dockSpace->direction = RIGHT;
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
                dockSpace->direction = BOTTOM;
                createDockSpace(dockSpace, &windowId);
                addWindow(dockSpace, panel);
            }

            ImGui::DockBuilderDockWindow(dockRepository.center.internalId, windowId);
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
        ImGui::DockBuilderDockWindow(d->internalId, d->nodeId);
        for (auto *l: panel->getChildren()) {
            if (dynamic_cast<DockSpacePanel *>(l)) {
                panel->appendChild(new DockSpacePanel(dynamic_cast<DockSpacePanel *>(l), d));
                return;
            }
        }
        panel->appendChild(new DockSpacePanel(nullptr, d));
    }

    void DockService::prepareForRemoval(DockDTO *dock, DockSpacePanel *dockSpacePanel) const {
        DockRepository &dockRepository = context.dockRepository;

        dockRepository.dockToRemove = dock;
        dockRepository.dockPanelToRemove = dockSpacePanel;
    }

    void DockService::updateForRemoval(AbstractPanel *panel) const {
        DockRepository &dockRepository = context.dockRepository;

        if (dockRepository.dockPanelToRemove != nullptr) {
            switch (dockRepository.dockToRemove->direction) {
                case LEFT:
                    dockRepository.left.erase(
                        std::ranges::remove(dockRepository.left, dockRepository.dockToRemove).begin(),
                        dockRepository.left.end());
                    break;
                case RIGHT:
                    dockRepository.right.erase(
                        std::ranges::remove(dockRepository.right, dockRepository.dockToRemove).begin(),
                        dockRepository.right.end());
                    break;
                case BOTTOM:
                    dockRepository.bottom.erase(
                        std::ranges::remove(dockRepository.bottom, dockRepository.dockToRemove).begin(),
                        dockRepository.bottom.end());
                    break;
                default:
                    break;
            }

            dockRepository.isInitialized = false;
            dockRepository.dockPanelToRemove->getView()->onRemove();
            auto &vec = panel->getChildren();
            vec.erase(std::ranges::remove(vec, dockRepository.dockPanelToRemove).begin(), vec.end());

            dockRepository.dockPanelToRemove->getView()->removeAllChildren();

            delete dockRepository.dockPanelToRemove->getView();
            delete dockRepository.dockPanelToRemove;
            delete dockRepository.dockToRemove;

            dockRepository.dockToRemove = nullptr;
            dockRepository.dockPanelToRemove = nullptr;
        }
    }
}
