#include "DockSpacePanel.h"

#include <string>
#include <iostream>

#include "AbstractDockPanel.h"
#include "../../../ApplicationContext.h"
#include "../../../common/Icons.h"
#include "../../dto/DockDTO.h"
#include "../../util/UIUtil.h"
#include "../../service/LogService.h"

namespace Metal {
    const ImVec2 DockSpacePanel::DEFAULT{-1.f, -1.f};
    const ImVec2 DockSpacePanel::MAX_SIZE{MAX_FLOAT, MAX_FLOAT};
    const ImVec2 DockSpacePanel::PIVOT{0.5f, 0.5f};
    const float DockSpacePanel::FRAME_SIZE = 25;
    const ImVec2 DockSpacePanel::MIN_SIZE{300.f, 300.f};

    void DockSpacePanel::onInitialize() {
        initializeView();
    }

    void DockSpacePanel::initializeView() {
        DockSpace *selectedSpace = getSelectedDockSpace();
        if (selectedSpace == nullptr) {
            view = nullptr;
            return;
        }

        const auto it = views.find(selectedSpace->index);
        if (it == views.end()) {
            auto *newView = selectedSpace->getPanel();
            newView->size = &size;
            newView->dock = selectedSpace;
            newView->position = &position;
            appendChild(newView);
            views.emplace(selectedSpace->index, newView);
            view = newView;
        } else {
            view = it->second;
            view->size = &size;
            view->dock = selectedSpace;
            view->position = &position;
        }

        padding.x = static_cast<float>(selectedSpace->paddingX);
        padding.y = static_cast<float>(selectedSpace->paddingY);
    }

    void DockSpacePanel::handleShortcut() const {
        if (view != nullptr) {
            const bool isHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);
            if (isHovered) {
                CTX.editorRepository.focusedShortcuts = view->getShortcuts();
                CTX.editorRepository.focusedWindowName = view->dock->name;
            }

            view->isWindowFocused = isHovered;
            if (view->isWindowFocused) {
                for (const auto &shortcut: CTX.editorRepository.focusedShortcuts) {
                    if (ImGui::IsKeyChordPressed(shortcut.keyChord)) {
                        LOG_INFO("Action called: " + shortcut.name);
                        shortcut.callback();
                    }
                }
            }
        }
    }

    void DockSpacePanel::onSync() {
        if (view == nullptr) {
            initializeView();
        }
        ImGui::SetNextWindowSizeConstraints(MIN_SIZE, MAX_SIZE);
        if (padding.x != DEFAULT.x || padding.y != DEFAULT.y) {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, padding);
            stylePushCount++;
        }
        if (!sizeInitialized && dock->sizeX > 0 && dock->sizeY > 0) {
            UIUtil::AUX_VEC2.x = dock->sizeX;
            UIUtil::AUX_VEC2.y = dock->sizeY;
            ImGui::SetNextWindowSize(UIUtil::AUX_VEC2);
            sizeInitialized = true;
        }
        beforeWindow();
        if (ImGui::Begin(dock->internalId.c_str(), &UIUtil::OPEN, FLAGS)) {
            handleShortcut();
            sizeInternal = ImGui::GetWindowSize();
            size.x = sizeInternal.x;
            size.y = sizeInternal.y;

            dock->sizeX = size.x;
            dock->sizeY = size.y;

            position = ImGui::GetWindowPos();
            renderHeader();
            if (view != nullptr) {
                view->onSync();
            }
        }
        ImGui::End();

        ImGui::PopStyleVar(stylePushCount);
        stylePushCount = 0;
    }

    AbstractDockPanel *DockSpacePanel::getView() const {
        return view;
    }

    void DockSpacePanel::renderHeader() {
        headerPadding.x = ImGui::GetStyle().FramePadding.x;

        if (ImGui::BeginMenuBar()) {
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, headerPadding);
            const bool isFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
            ImGui::PushStyleColor(ImGuiCol_TabActive,
                                  isFocused ? CTX.editorRepository.accent : CTX.themeService.palette0);

            if (ImGui::BeginTabBar((id + "dockTabs").c_str(), ImGuiTabBarFlags_AutoSelectNewTabs)) {
                for (auto *space: dock->dockSpaces) {
                    if (space == nullptr) {
                        continue;
                    }
                    const std::string label = space->icon + " " + space->name + id +
                                              std::to_string(space->index);

                    if (ImGui::BeginTabItem(label.c_str(), nullptr)) {
                        if (dock->selectedOption != space->index) {
                            dock->selectedOption = space->index;
                            initializeView();
                        }
                        ImGui::EndTabItem();
                    }
                }
                ImGui::SetNextItemWidth(23);
                if (ImGui::TabItemButton((Icons::add.c_str() + id + "addTab").c_str(),
                                         ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoReorder)) {
                    ImGui::OpenPopup((id + "NewTabDropdown").c_str());
                }

                if (ImGui::BeginPopup((id + "NewTabDropdown").c_str())) {
                    ImGui::Text("New Tab");
                    ImGui::Separator();
                    for (int i = 0; i <= 4; i++) {
                        DockSpace *option = DockSpace::GetOption(i);
                        if (option == nullptr) {
                            continue;
                        }
                        const bool exists = hasDockSpace(option->index);
                        const std::string label = option->icon + " " + option->name;
                        if (ImGui::MenuItem(label.c_str(), nullptr, false, !exists)) {
                            dock->dockSpaces.emplace_back(option);
                            dock->selectedOption = option->index;
                            initializeView();
                        }
                    }
                    ImGui::EndPopup();
                }

                ImGui::EndTabBar();
            }
            ImGui::PopStyleColor();
            ImGui::PopStyleVar();
            ImGui::EndMenuBar();
        }
    }

    DockSpace *DockSpacePanel::getSelectedDockSpace() const {
        if (dock->dockSpaces.empty()) {
            return nullptr;
        }

        for (auto *space: dock->dockSpaces) {
            if (space != nullptr && space->index == dock->selectedOption) {
                return space;
            }
        }

        return dock->dockSpaces.front();
    }

    bool DockSpacePanel::hasDockSpace(const int index) const {
        for (auto *space: dock->dockSpaces) {
            if (space != nullptr && space->index == index) {
                return true;
            }
        }
        return false;
    }

    void DockSpacePanel::beforeWindow() const {
        if (mainWindow != nullptr && mainWindow != this) {
            const ImVec2 &pos = mainWindow->getPosition();
            const ImVec2 &sze = mainWindow->getSize();
            UIUtil::AUX_VEC2.x = pos.x + sze.x * 0.5f;
            UIUtil::AUX_VEC2.y = pos.y + sze.y * 0.5f;
            ImGui::SetNextWindowPos(UIUtil::AUX_VEC2, ImGuiCond_FirstUseEver, PIVOT);
        }
    }

    ImVec2 &DockSpacePanel::getPosition() {
        return position;
    }

    ImVec2 &DockSpacePanel::getSize() {
        return sizeInternal;
    }
} // Metal
