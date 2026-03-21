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
#include "../../service/DockService.h"
#include "editor/service/HistoryService.h"
#include "core/DirectoryService.h"
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
            auto newView = selectedSpace->getPanel();
            newView->size = &size;
            newView->dock = selectedSpace;
            newView->position = &position;
            initializePanel(newView);

            newView->shortcuts.emplace_back("Undo", ImGuiMod_Ctrl | ImGuiKey_Z, [this] {
                historyService->undo();
            });
            newView->shortcuts.emplace_back("Redo", ImGuiMod_Ctrl | ImGuiKey_Y, [this] {
                historyService->redo();
            });
            newView->shortcuts.emplace_back("Save", ImGuiMod_Ctrl | ImGuiKey_S, [this] {
                directoryService->save();
            });

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
        const bool isHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);

        if (view != nullptr) {
            if (isHovered) {
                editorRepository->focusedShortcuts = view->getShortcuts();
                editorRepository->focusedWindowName =
                        UIUtil::GetDockSpaceIcon(view->dock->index) + " " + view->dock->name;
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
            ImGui::SetNextWindowSize(ImVec2(dock->sizeX, dock->sizeY));
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
                         (FLAGS & ~ImGuiWindowFlags_MenuBar) | ImGuiWindowFlags_NoBackground |
                         ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
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
            if (ImGui::BeginChild((dock->internalId + "island").c_str(),
                                  ImVec2(size.x - OUTSIDE_PADDING * 2, size.y - OUTSIDE_PADDING * 2),
                                  dock->isCenter ? ImGuiChildFlags_None : ImGuiChildFlags_AlwaysUseWindowPadding,
                                  ImGuiWindowFlags_None)) {
                if (!dock->isCenter) {
                    renderCustomHeader();
                }

                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,
                                    padding.x != DEFAULT.x ? padding : ImVec2(INSIDE_PADDING, INSIDE_PADDING));
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

    std::shared_ptr<AbstractDockPanel> DockSpacePanel::getView() const {
        return view;
    }

    void DockSpacePanel::renderCustomHeader() {
        const float headerPaddingY = (headerHeight - ImGui::GetFrameHeight()) * 0.5f;

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x, headerPaddingY));

        // Tab Bar Area
        ImGui::BeginGroup();

        const float availableWidth = ImGui::GetContentRegionAvail().x - headerHeight - ImGui::GetStyle().ItemSpacing.x;

        if (ImGui::BeginChild((id + "tabs_scroll").c_str(), ImVec2(availableWidth, headerHeight), ImGuiChildFlags_None,
                              ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoBackground)) {
            const bool isFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

            for (auto *space: dock->dockSpaces) {
                if (space == nullptr) continue;

                const bool isSelected = (dock->selectedOption == space->index);

                if (isSelected) {
                    ImGui::PushStyleColor(ImGuiCol_Button,
                                          isFocused ? editorRepository->accent : themeService->palette3);
                } else {
                    ImGui::PushStyleColor(ImGuiCol_Button, themeService->palette3);
                }

                if (ImGui::Button(
                    (UIUtil::GetDockSpaceIcon(space->index) + space->name + id + std::to_string(space->index)).c_str(),
                    ImVec2(0, headerHeight))) {
                    dock->selectedOption = space->index;
                    initializeView();
                }

                if (!dock->isCenter) {
                    ImGui::SameLine(0, 0);
                    ImGui::PushStyleColor(ImGuiCol_Button, themeService->palette0);
                    ImGui::PushStyleColor(ImGuiCol_Border, themeService->palette0);

                    if (UIUtil::ButtonSimple(Icons::close + "##close" + std::to_string(space->index) + id, headerHeight,
                                             headerHeight)) {
                        int indexToRemove = -1;
                        for (int i = 0; i < (int) dock->dockSpaces.size(); i++) {
                            if (dock->dockSpaces[i]->index == space->index) {
                                indexToRemove = i;
                                break;
                            }
                        }
                        if (indexToRemove != -1) {
                            if (dock->dockSpaces.size() == 1) {
                                dockService->removeDock(dock);
                            } else {
                                dock->dockSpaces.erase(dock->dockSpaces.begin() + indexToRemove);
                                if (isSelected && !dock->dockSpaces.empty()) {
                                    dock->selectedOption = dock->dockSpaces.front()->index;
                                    initializeView();
                                }
                            }
                        }
                    }
                    ImGui::PopStyleColor(2);
                }

                ImGui::PopStyleColor();
                ImGui::SameLine();
            }
        }
        ImGui::EndChild();

        ImGui::SameLine();

        if (UIUtil::ButtonSimple(Icons::add + "##addTab" + id, headerHeight, headerHeight)) {
            ImGui::OpenPopup((id + "NewTabDropdown").c_str());
        }
        if (UIUtil::BeginPopupContext(id + "NewTabDropdown")) {
            ImGui::Text("New Tab");
            ImGui::Separator();
            for (auto *option: DockSpace::OPTIONS_LIST) {
                const bool exists = hasDockSpace(option->index);
                const std::string label = UIUtil::GetDockSpaceIcon(option->index) + " " + option->name;
                if (ImGui::MenuItem(label.c_str(), nullptr, false, !exists)) {
                    dock->dockSpaces.emplace_back(option);
                    dock->selectedOption = option->index;
                    initializeView();
                }
            }
            UIUtil::EndPopupContext();
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
        if (mainWindow != nullptr && mainWindow.get() != this) {
            const ImVec2 &pos = mainWindow->getPosition();
            const ImVec2 &sze = mainWindow->getSize();
            ImGui::SetNextWindowPos(ImVec2(
                                        pos.x + sze.x * 0.5f,
                                        pos.y + sze.y * 0.5f), ImGuiCond_FirstUseEver, PIVOT);
        }
    }

    ImVec2 &DockSpacePanel::getPosition() {
        return position;
    }

    ImVec2 &DockSpacePanel::getSize() {
        return sizeInternal;
    }
} // Metal
