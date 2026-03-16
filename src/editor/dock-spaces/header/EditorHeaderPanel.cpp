#include "EditorHeaderPanel.h"
#include "../../util/UIUtil.h"
#include "../../../core/DirectoryService.h"
#include "../../../core/WindowService.h"
#include "../../../engine/EngineContext.h"
#include "../../service/DockService.h"
#include "editor/service/HistoryService.h"
#include "editor/service/ThemeService.h"
#define HEIGHT 30.0f
#define BUTTON_SIZE 25.0f

namespace Metal {
    void EditorHeaderPanel::onSync() {
        ImGui::PushStyleColor(ImGuiCol_Button, themeService->palette1);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        ImGui::SetCursorPosX(5);
        ImGui::SetCursorPosY(0);
        if (UIUtil::ButtonSimple(Icons::save + "##save", BUTTON_SIZE, BUTTON_SIZE)) {
            directoryService->save();
        }
        ImGui::SameLine(0, 4);
        const bool canUndo = historyService->canUndo();
        if (!canUndo) ImGui::BeginDisabled();
        if (UIUtil::ButtonSimple(Icons::undo + "##undo", BUTTON_SIZE, BUTTON_SIZE)) {
            historyService->undo();
        }
        if (!canUndo) ImGui::EndDisabled();

        ImGui::SameLine();
        const bool canRedo = historyService->canRedo();
        if (!canRedo) ImGui::BeginDisabled();
        if (UIUtil::ButtonSimple(Icons::redo + "##redo", BUTTON_SIZE, BUTTON_SIZE)) {
            historyService->redo();
        }
        if (!canRedo) ImGui::EndDisabled();
        ImGui::SameLine(0, 4);

        renderDockAdders();
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
    }

    void EditorHeaderPanel::renderMenu(const char *label, std::function<void()> itemsFunc) {
        if (ImGui::Button(label, ImVec2(0, HEIGHT))) {
            ImGui::OpenPopup(label);
        }
        if (ImGui::BeginPopup(label)) {
            itemsFunc();
            ImGui::EndPopup();
        }
    }

    void EditorHeaderPanel::renderDockAdders() {
        const float spacing = 2.0f;

        ImGui::SetCursorPosY(0);

        if (UIUtil::ButtonSimple(Icons::split_scene_left + "##addLeft", BUTTON_SIZE, BUTTON_SIZE)) {
            dockService->addLeftDock();
        }
        UIUtil::RenderTooltip("Add Left Dock");
        ImGui::SameLine(0, spacing);

        if (UIUtil::ButtonSimple(Icons::split_scene_down + "##addBottom", BUTTON_SIZE, BUTTON_SIZE)) {
            dockService->addBottomDock();
        }
        UIUtil::RenderTooltip("Add Bottom Dock");
        ImGui::SameLine(0, spacing);

        if (UIUtil::ButtonSimple(Icons::split_scene_right + "##addRight", BUTTON_SIZE, BUTTON_SIZE)) {
            dockService->addRightDock();
        }
        UIUtil::RenderTooltip("Add Right Dock");
    }
}
