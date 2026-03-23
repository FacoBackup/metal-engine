#include "EditorHeaderPanel.h"
#include "editor/ui/UIUtil.h"
#include "../../repository/EditorRepository.h"
#include "../../../core/DirectoryService.h"
#include "../../../core/WindowService.h"
#include "engine/EngineContext.h"
#include "../../service/DockService.h"
#include "../EditorPanel.h"
#include "editor/service/HistoryService.h"
#include "editor/service/ThemeService.h"
#include "../../../ApplicationEventContext.h"

namespace Metal {
    void EditorHeaderPanel::onSync() {
        float windowWidth = ImGui::GetIO().DisplaySize.x;

        WindowService::WindowControlRects rects{};

        float buttonWidth = 45.0f;
        float xPos = windowWidth - buttonWidth;

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
        // Close Button
        rects.closeButton = {
            static_cast<int>(xPos), 0, static_cast<int>(buttonWidth), static_cast<int>(EditorPanel::HEADER_HEIGHT)
        };
        RenderWindowControl(Icons::close, "close", xPos, buttonWidth, IM_COL32(232, 17, 35, 255), [this] {
            windowService->close();
        });

        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, themeService->palette0);

        // Maximize/Restore Button
        xPos -= buttonWidth;
        rects.maximizeButton = {
            static_cast<int>(xPos), 0, static_cast<int>(buttonWidth), static_cast<int>(EditorPanel::HEADER_HEIGHT)
        };
        bool isMaximized = windowService->isMaximized();
        RenderWindowControl(isMaximized ? Icons::filter_none : Icons::crop_square, "maximize", xPos, buttonWidth,
                            IM_COL32(255, 255, 255, 30), [this] {
                                windowService->maximize();
                            });

        // Minimize Button
        xPos -= buttonWidth;
        rects.minimizeButton = {
            (int) xPos, 0, static_cast<int>(buttonWidth), static_cast<int>(EditorPanel::HEADER_HEIGHT)
        };
        RenderWindowControl(Icons::remove, "minimize", xPos, buttonWidth, IM_COL32(255, 255, 255, 30), [this] {
            windowService->minimize();
        });

        // Play/Stop Button
        xPos -= 30.0f;
        ImGui::SetCursorPosX(xPos);
        ImGui::SetCursorPosY((EditorPanel::HEADER_HEIGHT - 25.0f) * 0.5f);
        const bool isPlaying = editorRepository->isPlaying;
        const std::string icon = isPlaying ? Icons::stop : Icons::play_arrow;
        const ImVec4 color = isPlaying ? ImVec4(0.957f, 0.263f, 0.212f, 1.0f) : ImVec4(0.298f, 0.686f, 0.314f, 1.0f);
        const std::string tooltip = isPlaying ? "Stop" : "Play";

        if (UIUtil::RenderButtonSolid("playstop", icon, 25.0f, color, 4.0f)) {
            if (isPlaying) {
                ApplicationEventContext::dispatch("Stop");
                editorRepository->isPlaying = false;
            } else {
                ApplicationEventContext::dispatch("Play");
                editorRepository->isPlaying = true;
            }
        }
        UIUtil::RenderTooltip(tooltip);

        ImGui::PopStyleColor(1);
        ImGui::PopStyleVar();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        ImGui::SetCursorPosX(5);
        ImGui::SetCursorPosY(0);
        rects.saveButton = {
            5, 0, static_cast<int>(EditorPanel::HEADER_HEIGHT), static_cast<int>(EditorPanel::HEADER_HEIGHT)
        };
        if (UIUtil::ButtonSimple(Icons::save + "##save", EditorPanel::HEADER_HEIGHT, EditorPanel::HEADER_HEIGHT)) {
            directoryService->save();
        }
        ImGui::SameLine(0, 4);
        const bool canUndo = historyService->canUndo();
        if (!canUndo) ImGui::BeginDisabled();
        rects.undoButton = {
            static_cast<int>(ImGui::GetCursorPosX()), 0, static_cast<int>(EditorPanel::HEADER_HEIGHT),
            static_cast<int>(EditorPanel::HEADER_HEIGHT)
        };
        if (UIUtil::ButtonSimple(Icons::undo + "##undo", EditorPanel::HEADER_HEIGHT, EditorPanel::HEADER_HEIGHT)) {
            historyService->undo();
        }
        if (!canUndo) ImGui::EndDisabled();

        ImGui::SameLine();
        const bool canRedo = historyService->canRedo();
        if (!canRedo) ImGui::BeginDisabled();
        rects.redoButton = {
            static_cast<int>(ImGui::GetCursorPosX()), 0, static_cast<int>(EditorPanel::HEADER_HEIGHT),
            static_cast<int>(EditorPanel::HEADER_HEIGHT)
        };
        if (UIUtil::ButtonSimple(Icons::redo + "##redo", EditorPanel::HEADER_HEIGHT, EditorPanel::HEADER_HEIGHT)) {
            historyService->redo();
        }
        if (!canRedo) ImGui::EndDisabled();
        ImGui::SameLine(0, 4);

        renderDockAdders(rects);

        float leftSideEnd = ImGui::GetCursorPosX();
        rects.dragArea = {
            static_cast<int>(leftSideEnd), 0, static_cast<int>(xPos - leftSideEnd),
            static_cast<int>(EditorPanel::HEADER_HEIGHT)
        };

        windowService->setWindowControlRects(rects);

        ImGui::PopStyleVar();
        ImGui::PopStyleColor(2);
    }

    void EditorHeaderPanel::RenderMenu(const char *label, const std::function<void()> &itemsFunc) {
        if (ImGui::Button(label, ImVec2(0, EditorPanel::HEADER_HEIGHT))) {
            ImGui::OpenPopup(label);
        }
        if (ImGui::BeginPopup(label)) {
            itemsFunc();
            ImGui::EndPopup();
        }
    }

    void EditorHeaderPanel::renderDockAdders(WindowService::WindowControlRects &rects) const {
        const float spacing = 2.0f;

        ImGui::SetCursorPosY(0);

        rects.dockAdders.push_back({
            static_cast<int>(ImGui::GetCursorPosX()), 0, static_cast<int>(EditorPanel::HEADER_HEIGHT),
            static_cast<int>(EditorPanel::HEADER_HEIGHT)
        });
        if (UIUtil::ButtonSimple(Icons::split_scene_left + "##addLeft", EditorPanel::HEADER_HEIGHT,
                                 EditorPanel::HEADER_HEIGHT)) {
            dockService->addLeftDock();
        }
        UIUtil::RenderTooltip("Add Left Dock");
        ImGui::SameLine(0, spacing);

        rects.dockAdders.push_back({
            static_cast<int>(ImGui::GetCursorPosX()), 0, static_cast<int>(EditorPanel::HEADER_HEIGHT),
            static_cast<int>(EditorPanel::HEADER_HEIGHT)
        });
        if (UIUtil::ButtonSimple(Icons::split_scene_down + "##addBottom", EditorPanel::HEADER_HEIGHT,
                                 EditorPanel::HEADER_HEIGHT)) {
            dockService->addBottomDock();
        }
        UIUtil::RenderTooltip("Add Bottom Dock");
        ImGui::SameLine(0, spacing);

        rects.dockAdders.push_back({
            static_cast<int>(ImGui::GetCursorPosX()), 0, static_cast<int>(EditorPanel::HEADER_HEIGHT),
            static_cast<int>(EditorPanel::HEADER_HEIGHT)
        });
        if (UIUtil::ButtonSimple(Icons::split_scene_right + "##addRight", EditorPanel::HEADER_HEIGHT,
                                 EditorPanel::HEADER_HEIGHT)) {
            dockService->addRightDock();
        }
        UIUtil::RenderTooltip("Add Right Dock");
    }

    void EditorHeaderPanel::RenderWindowControl(const std::string &icon, const char *id, float xPos, float width,
                                                ImU32 hoverColor, std::function<void()> action) {
        ImGui::SetCursorPosX(xPos);
        ImGui::SetCursorPosY(0);
        if (ImGui::Button((icon + "##" + id).c_str(), ImVec2(width, EditorPanel::HEADER_HEIGHT))) {
            action();
        }
        if (ImGui::IsItemHovered()) {
            ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), hoverColor);
        }
    }
}
