#include "EditorHeaderPanel.h"
#include "../../util/UIUtil.h"
#include "../../../core/DirectoryService.h"
#include "../../../core/WindowService.h"
#include "../../../engine/EngineContext.h"
#include "../../service/DockService.h"
#include "editor/EditorPanel.h"
#include "editor/service/HistoryService.h"
#include "editor/service/ThemeService.h"

namespace Metal {
    void EditorHeaderPanel::onSync() {
        float windowWidth = ImGui::GetIO().DisplaySize.x;

        WindowService::WindowControlRects rects{};
        rects.dragArea = {0, 0, static_cast<int>(windowWidth), static_cast<int>(EditorPanel::HEADER_HEIGHT)};

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

        windowService->setWindowControlRects(rects);

        ImGui::PopStyleColor(1);
        ImGui::PopStyleVar();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        ImGui::SetCursorPosX(5);
        ImGui::SetCursorPosY(0);
        if (UIUtil::ButtonSimple(Icons::save + "##save", EditorPanel::HEADER_HEIGHT, EditorPanel::HEADER_HEIGHT)) {
            directoryService->save();
        }
        ImGui::SameLine(0, 4);
        const bool canUndo = historyService->canUndo();
        if (!canUndo) ImGui::BeginDisabled();
        if (UIUtil::ButtonSimple(Icons::undo + "##undo", EditorPanel::HEADER_HEIGHT, EditorPanel::HEADER_HEIGHT)) {
            historyService->undo();
        }
        if (!canUndo) ImGui::EndDisabled();

        ImGui::SameLine();
        const bool canRedo = historyService->canRedo();
        if (!canRedo) ImGui::BeginDisabled();
        if (UIUtil::ButtonSimple(Icons::redo + "##redo", EditorPanel::HEADER_HEIGHT, EditorPanel::HEADER_HEIGHT)) {
            historyService->redo();
        }
        if (!canRedo) ImGui::EndDisabled();
        ImGui::SameLine(0, 4);

        renderDockAdders();
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

    void EditorHeaderPanel::renderDockAdders() const {
        const float spacing = 2.0f;

        ImGui::SetCursorPosY(0);

        if (UIUtil::ButtonSimple(Icons::split_scene_left + "##addLeft", EditorPanel::HEADER_HEIGHT,
                                 EditorPanel::HEADER_HEIGHT)) {
            dockService->addLeftDock();
        }
        UIUtil::RenderTooltip("Add Left Dock");
        ImGui::SameLine(0, spacing);

        if (UIUtil::ButtonSimple(Icons::split_scene_down + "##addBottom", EditorPanel::HEADER_HEIGHT,
                                 EditorPanel::HEADER_HEIGHT)) {
            dockService->addBottomDock();
        }
        UIUtil::RenderTooltip("Add Bottom Dock");
        ImGui::SameLine(0, spacing);

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
