#include <imgui_internal.h>
#include <algorithm>
#include "DockService.h"
#include "../dto/DockDefinition.h"
#include "editor/ui/abstract/AbstractPanel.h"
#include "../ui/docks/AbstractDockPanel.h"
#include "../ui/docks/DockSpacePanel.h"
#include "ApplicationContext.h"
#include "editor/dto/DockSpace.h"

namespace Metal {
    void DockService::onInitialize() {
        if (!dockRepository->getLeft().empty() || !dockRepository->getRight().empty() || !dockRepository->getBottom().
            empty()) {
            return;
        }

        auto rightT = std::make_shared<DockDefinition>(&DockSpace::WORLD);
        auto leftT = std::make_shared<DockDefinition>(&DockSpace::REPOSITORIES);
        auto leftB = std::make_shared<DockDefinition>(&DockSpace::INSPECTOR);
        auto rightB = std::make_shared<DockDefinition>(&DockSpace::CONSOLE);
        auto b = std::make_shared<DockDefinition>(&DockSpace::FILES);

        dockRepository->getCenter()->sizeRatioForNodeAtDir = 0.5f;
        rightT->sizeRatioForNodeAtDir = 0.25f;
        leftT->sizeRatioForNodeAtDir = 0.2f;
        leftB->sizeRatioForNodeAtDir = 0.5f;
        rightB->sizeRatioForNodeAtDir = 0.25f;
        b->sizeRatioForNodeAtDir = 0.25f;

        dockRepository->getRight().push_back(rightT);
        dockRepository->getRight().push_back(rightB);

        dockRepository->getLeft().push_back(leftT);
        dockRepository->getLeft().push_back(leftB);

        dockRepository->getBottom().push_back(b);
    }

    void DockService::buildViews(ImGuiID windowId, AbstractPanel *panel) {
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable) {
            if (isInitialized) {
                return;
            }
            isInitialized = true;

            std::vector<std::shared_ptr<DockDefinition> > allDocks;
            allDocks.insert(allDocks.end(), dockRepository->getLeft().begin(), dockRepository->getLeft().end());
            allDocks.insert(allDocks.end(), dockRepository->getRight().begin(), dockRepository->getRight().end());
            allDocks.insert(allDocks.end(), dockRepository->getBottom().begin(), dockRepository->getBottom().end());
            allDocks.push_back(dockRepository->getCenter());

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

            for (size_t i = 0; i < dockRepository->getLeft().size(); i++) {
                auto dockSpace = dockRepository->getLeft()[i];
                if (i == 0) {
                    dockSpace->origin = nullptr;
                    dockSpace->outAtOppositeDir = nullptr;
                    dockSpace->splitDir = ImGuiDir_Left;
                } else {
                    auto previous = dockRepository->getLeft()[i - 1];
                    dockSpace->origin = previous;
                    dockSpace->outAtOppositeDir = previous;
                    dockSpace->splitDir = ImGuiDir_Down;
                }
                createDockSpace(dockSpace, &windowId);
                addWindow(dockSpace, panel);
            }

            for (size_t i = 0; i < dockRepository->getRight().size(); i++) {
                auto dockSpace = dockRepository->getRight()[i];
                if (i == 0) {
                    dockSpace->origin = nullptr;
                    dockSpace->outAtOppositeDir = nullptr;
                    dockSpace->splitDir = ImGuiDir_Right;
                } else {
                    auto previous = dockRepository->getRight()[i - 1];
                    dockSpace->origin = previous;
                    dockSpace->outAtOppositeDir = previous;
                    dockSpace->splitDir = ImGuiDir_Down;
                }
                createDockSpace(dockSpace, &windowId);
                addWindow(dockSpace, panel);
            }

            for (size_t i = 0, bottomSize = dockRepository->getBottom().size(); i < bottomSize; i++) {
                auto dockSpace = dockRepository->getBottom()[i];
                if (i == 0) {
                    dockSpace->origin = nullptr;
                    dockSpace->outAtOppositeDir = nullptr;
                    dockSpace->splitDir = ImGuiDir_Down;
                } else {
                    auto previous = dockRepository->getBottom()[i - 1];
                    dockSpace->origin = previous;
                    dockSpace->outAtOppositeDir = previous;
                    dockSpace->splitDir = ImGuiDir_Right;
                }
                createDockSpace(dockSpace, &windowId);
                addWindow(dockSpace, panel);
            }

            dockRepository->getCenter()->nodeId = windowId;
            addWindow(dockRepository->getCenter(), panel);

            ImGui::DockBuilderDockWindow(dockRepository->getCenter()->internalId.c_str(), windowId);
            ImGui::DockBuilderFinish(windowId);
        }
    }

    void DockService::removeDock(std::shared_ptr<DockDefinition> dock) {
        if (dock == nullptr || dock->isCenter()) {
            return;
        }

        auto removeIt = [&](std::vector<std::shared_ptr<DockDefinition> > &vec) {
            auto it = std::find(vec.begin(), vec.end(), dock);
            if (it != vec.end()) {
                vec.erase(it);
                return true;
            }
            return false;
        };

        bool removed = false;
        if (removeIt(dockRepository->getLeft())) removed = true;
        else if (removeIt(dockRepository->getRight())) removed = true;
        else if (removeIt(dockRepository->getBottom())) removed = true;

        if (removed) {
            isInitialized = false;
        }
    }

    void DockService::addLeftDock() {
        auto d = std::make_shared<DockDefinition>(&DockSpace::CONSOLE);
        d->sizeRatioForNodeAtDir = 0.25f;
        dockRepository->getLeft().push_back(d);
        isInitialized = false;
    }

    void DockService::addBottomDock() {
        auto d = std::make_shared<DockDefinition>(&DockSpace::CONSOLE);
        d->sizeRatioForNodeAtDir = 0.25f;
        dockRepository->getBottom().push_back(d);
        isInitialized = false;
    }

    void DockService::addRightDock() {
        auto d = std::make_shared<DockDefinition>(&DockSpace::CONSOLE);
        d->sizeRatioForNodeAtDir = 0.25f;
        dockRepository->getRight().push_back(d);
        isInitialized = false;
    }

    void DockService::createDockSpace(std::shared_ptr<DockDefinition> dockSpace, ImGuiID *dockMainId) {
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

    void DockService::addWindow(std::shared_ptr<DockDefinition> d, AbstractPanel *panel) {
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
