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
        auto rightT = std::make_shared<DockDTO>(&DockSpace::WORLD);
        auto leftT = std::make_shared<DockDTO>(&DockSpace::REPOSITORIES);
        auto leftB = std::make_shared<DockDTO>(&DockSpace::INSPECTOR);
        auto rightB = std::make_shared<DockDTO>(&DockSpace::CONSOLE);
        auto b = std::make_shared<DockDTO>(&DockSpace::FILES);

        center->isCenter = true;
        center->sizeRatioForNodeAtDir = 0.5f;
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

            std::vector<std::shared_ptr<DockDTO>> allDocks;
            allDocks.insert(allDocks.end(), left.begin(), left.end());
            allDocks.insert(allDocks.end(), right.begin(), right.end());
            allDocks.insert(allDocks.end(), bottom.begin(), bottom.end());
            allDocks.push_back(center);

            auto &children = panel->getChildren();
            children.erase(
                std::remove_if(children.begin(), children.end(), [&](const std::shared_ptr<AbstractPanel> &child) {
                    auto dsPanel = std::dynamic_pointer_cast<DockSpacePanel>(child);
                    if (dsPanel == nullptr) return false;
                    auto it = std::find(allDocks.begin(), allDocks.end(), dsPanel->getDock());
                    return it == allDocks.end();
                }),
                children.end());

            ImGui::DockBuilderRemoveNode(windowId);
            ImGui::DockBuilderAddNode(windowId, ImGuiDockNodeFlags_NoTabBar);
            ImGui::DockBuilderSetNodeSize(windowId, ImGui::GetMainViewport()->Size);

            for (size_t i = 0; i < left.size(); i++) {
                auto dockSpace = left[i];
                if (i == 0) {
                    dockSpace->origin = nullptr;
                    dockSpace->outAtOppositeDir = nullptr;
                    dockSpace->splitDir = ImGuiDir_Left;
                } else {
                    auto previous = left[i - 1];
                    dockSpace->origin = previous;
                    dockSpace->outAtOppositeDir = previous;
                    dockSpace->splitDir = ImGuiDir_Down;
                }
                createDockSpace(dockSpace, &windowId);
                addWindow(dockSpace, panel);
            }

            for (size_t i = 0; i < right.size(); i++) {
                auto dockSpace = right[i];
                if (i == 0) {
                    dockSpace->origin = nullptr;
                    dockSpace->outAtOppositeDir = nullptr;
                    dockSpace->splitDir = ImGuiDir_Right;
                } else {
                    auto previous = right[i - 1];
                    dockSpace->origin = previous;
                    dockSpace->outAtOppositeDir = previous;
                    dockSpace->splitDir = ImGuiDir_Down;
                }
                createDockSpace(dockSpace, &windowId);
                addWindow(dockSpace, panel);
            }

            for (size_t i = 0, bottomSize = bottom.size(); i < bottomSize; i++) {
                auto dockSpace = bottom[i];
                if (i == 0) {
                    dockSpace->origin = nullptr;
                    dockSpace->outAtOppositeDir = nullptr;
                    dockSpace->splitDir = ImGuiDir_Down;
                } else {
                    auto previous = bottom[i - 1];
                    dockSpace->origin = previous;
                    dockSpace->outAtOppositeDir = previous;
                    dockSpace->splitDir = ImGuiDir_Right;
                }
                createDockSpace(dockSpace, &windowId);
                addWindow(dockSpace, panel);
            }

            center->nodeId = windowId;
            addWindow(center, panel);

            ImGui::DockBuilderDockWindow(center->internalId.c_str(), windowId);
            ImGui::DockBuilderFinish(windowId);
        }
    }

    void DockService::removeDock(std::shared_ptr<DockDTO> dock) {
        if (dock == nullptr || dock->isCenter) {
            return;
        }

        auto removeIt = [&](std::vector<std::shared_ptr<DockDTO>> &vec) {
            auto it = std::find(vec.begin(), vec.end(), dock);
            if (it != vec.end()) {
                vec.erase(it);
                return true;
            }
            return false;
        };

        bool removed = false;
        if (removeIt(left)) removed = true;
        else if (removeIt(right)) removed = true;
        else if (removeIt(bottom)) removed = true;

        if (removed) {
            isInitialized = false;
        }
    }

    void DockService::addLeftDock() {
        auto d = std::make_shared<DockDTO>(&DockSpace::CONSOLE);
        d->sizeRatioForNodeAtDir = 0.25f;
        left.push_back(d);
        isInitialized = false;
    }

    void DockService::addBottomDock() {
        auto d = std::make_shared<DockDTO>(&DockSpace::CONSOLE);
        d->sizeRatioForNodeAtDir = 0.25f;
        bottom.push_back(d);
        isInitialized = false;
    }

    void DockService::addRightDock() {
        auto d = std::make_shared<DockDTO>(&DockSpace::CONSOLE);
        d->sizeRatioForNodeAtDir = 0.25f;
        right.push_back(d);
        isInitialized = false;
    }

    void DockService::createDockSpace(std::shared_ptr<DockDTO> dockSpace, ImGuiID *dockMainId) {
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

    void DockService::addWindow(std::shared_ptr<DockDTO> d, AbstractPanel *panel) {
        ImGui::DockBuilderDockWindow(d->internalId.c_str(), d->nodeId);
        for (auto &l: panel->getChildren()) {
            auto dsPanel = std::dynamic_pointer_cast<DockSpacePanel>(l);
            if (dsPanel != nullptr && dsPanel->getDock() == d) {
                return;
            }
        }
        
        for (auto &l: panel->getChildren()) {
            auto dsPanel = std::dynamic_pointer_cast<DockSpacePanel>(l);
            if (dsPanel != nullptr) {
                panel->initializePanel<DockSpacePanel>(true, dsPanel, d);
                return;
            }
        }
        panel->initializePanel<DockSpacePanel>(true, nullptr, d);
    }
}
