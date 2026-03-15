#include "DockSpacePanel.h"

#include <string>
#include <iostream>

#include "AbstractDockPanel.h"
#include "../../../ApplicationContext.h"
#include "../../../common/Icons.h"
#include "../../dto/DockDTO.h"
#include "../../util/UIUtil.h"
#include "../../../common/LoggerUtil.h"
#include "../../repository/EditorRepository.h"
#include "../../service/ThemeService.h"
#define BORDER_RADIUS  6.0f
#define OUTSIDE_PADDING  2.0f
#define INSIDE_PADDING  4.0f

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
            initializePanel(newView);
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
                editorRepository->focusedShortcuts = view->getShortcuts();
                editorRepository->focusedWindowName = view->dock->name;
            }

            view->isWindowFocused = isHovered;
            if (view->isWindowFocused) {
                for (const auto &shortcut: editorRepository->focusedShortcuts) {
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
        if (!sizeInitialized && dock->sizeX > 0 && dock->sizeY > 0) {
            UIUtil::AUX_VEC2.x = dock->sizeX;
            UIUtil::AUX_VEC2.y = dock->sizeY;
            ImGui::SetNextWindowSize(UIUtil::AUX_VEC2);
            sizeInitialized = true;
        }
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, BORDER_RADIUS);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_Border, themeService->palette3);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        stylePushCount = 3;
        styleColorPushCount = 2;
        beforeWindow();
        if (ImGui::Begin(dock->internalId.c_str(), &UIUtil::OPEN,
                         (FLAGS & ~ImGuiWindowFlags_MenuBar) | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
            handleShortcut();
            sizeInternal = ImGui::GetWindowSize();
            size.x = sizeInternal.x;
            size.y = sizeInternal.y;

            dock->sizeX = size.x;
            dock->sizeY = size.y;

            position = ImGui::GetWindowPos();

            ImGui::SetCursorPos(ImVec2(OUTSIDE_PADDING, OUTSIDE_PADDING));
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, BORDER_RADIUS);
            ImGui::PushStyleColor(ImGuiCol_ChildBg, themeService->palette0);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(INSIDE_PADDING, INSIDE_PADDING));
            if (ImGui::BeginChild((dock->internalId + "island").c_str(), ImVec2(size.x - OUTSIDE_PADDING * 2, size.y - OUTSIDE_PADDING * 2),
                                  dock->isCenter ? ImGuiChildFlags_None : ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_None)) {
                if (!dock->isCenter) {
                    renderCustomHeader();
                }

                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, padding.x != DEFAULT.x ? padding : ImVec2(INSIDE_PADDING, INSIDE_PADDING));
                if (ImGui::BeginChild((dock->internalId + "content").c_str(), ImVec2(0, 0), ImGuiChildFlags_None,
                                      ImGuiWindowFlags_None)) {
                    if (view != nullptr) {
                        view->onSync();
                    }
                }
                ImGui::EndChild();
                ImGui::PopStyleVar();
            }
            ImGui::EndChild();
            ImGui::PopStyleColor();
            ImGui::PopStyleVar(2);
        }
        ImGui::End();

        ImGui::PopStyleColor(styleColorPushCount);
        ImGui::PopStyleVar(stylePushCount);
        stylePushCount = 0;
        styleColorPushCount = 0;
    }

    AbstractDockPanel *DockSpacePanel::getView() const {
        return view;
    }

    void DockSpacePanel::renderCustomHeader() {
        const float buttonWidth = 23.0f;
        const float headerPaddingY = (headerHeight - ImGui::GetFrameHeight()) * 0.5f;

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x, headerPaddingY));

        // Tab Bar Area
        ImGui::BeginGroup();

        const float availableWidth = ImGui::GetContentRegionAvail().x - buttonWidth - ImGui::GetStyle().ItemSpacing.x;

        if (ImGui::BeginChild((id + "tabs_scroll").c_str(), ImVec2(availableWidth, headerHeight), ImGuiChildFlags_None,
                              ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoBackground)) {
            const bool isFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

            for (auto *space: dock->dockSpaces) {
                if (space == nullptr) continue;

                const std::string label = space->icon + " " + space->name + "##" + std::to_string(space->index);
                const bool isSelected = (dock->selectedOption == space->index);

                if (isSelected) {
                    ImGui::PushStyleColor(ImGuiCol_Button,
                                          isFocused ? editorRepository->accent : themeService->palette3);
                } else {
                    ImGui::PushStyleColor(ImGuiCol_Button, themeService->palette3);
                }

                if (ImGui::Button(label.c_str(), ImVec2(0, headerHeight))) {
                    dock->selectedOption = space->index;
                    initializeView();
                }

                if (dock->dockSpaces.size() > 1) {
                    ImGui::SameLine(0, 0);
                    ImGui::PushStyleColor(ImGuiCol_Button, isSelected ? (isFocused ? editorRepository->accent : themeService->palette3) : themeService->palette3);
                    if (ImGui::Button((Icons::close + "##close" + std::to_string(space->index) + id).c_str(), ImVec2(0, headerHeight))) {
                        int indexToRemove = -1;
                        for (int i = 0; i < dock->dockSpaces.size(); i++) {
                            if (dock->dockSpaces[i]->index == space->index) {
                                indexToRemove = i;
                                break;
                            }
                        }
                        if (indexToRemove != -1) {
                            dock->dockSpaces.erase(dock->dockSpaces.begin() + indexToRemove);
                            if (isSelected && !dock->dockSpaces.empty()) {
                                dock->selectedOption = dock->dockSpaces.front()->index;
                                initializeView();
                            }
                        }
                    }
                    ImGui::PopStyleColor();
                }

                ImGui::PopStyleColor();
                ImGui::SameLine();
            }
        }
        ImGui::EndChild();

        ImGui::SameLine();

        if (UIUtil::ButtonSimple(Icons::add + "##addTab" + id, buttonWidth, headerHeight)) {
            ImGui::OpenPopup((id + "NewTabDropdown").c_str());
        }
        if (ImGui::BeginPopup((id + "NewTabDropdown").c_str())) {
            ImGui::Text("New Tab");
            ImGui::Separator();
            for (auto *option: DockSpace::OPTIONS_LIST) {
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

        ImGui::EndGroup();
        ImGui::PopStyleVar();
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
