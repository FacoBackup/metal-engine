#include "EditorPanel.h"
#include "common/UIUtil.h"
#include "../common/runtime/ApplicationContext.h"

namespace Metal {
    int  EditorPanel::FLAGS = ImGuiWindowFlags_NoDocking |
                              ImGuiWindowFlags_NoTitleBar |
                              ImGuiWindowFlags_NoCollapse |
                              ImGuiWindowFlags_NoResize |
                              ImGuiWindowFlags_NoMove |
                              ImGuiWindowFlags_NoBringToFrontOnFocus |
                              ImGuiWindowFlags_NoNavFocus;
    const char *EditorPanel::NAME = "##main_window";
    const char *EditorPanel::NAME_HEADER = "##header_window";
    ImVec2 EditorPanel::CENTER(0.0f, 0.0f);
    float EditorPanel::HEADER_HEIGHT = 62;


    void EditorPanel::renderDockSpaces() {
        ImGuiViewport *viewport = ImGui::GetMainViewport();
        renderHeader(viewport);

        // Begin window
        {
            UIUtil::AUX_VEC2.x = viewport->Pos.x;
            UIUtil::AUX_VEC2.y = viewport->Pos.y + HEADER_HEIGHT;
            ImGui::SetNextWindowPos(UIUtil::AUX_VEC2);

            UIUtil::AUX_VEC2.x = viewport->Size.x;
            UIUtil::AUX_VEC2.y = viewport->Size.y - HEADER_HEIGHT;
            ImGui::SetNextWindowSize(UIUtil::AUX_VEC2);
            ImGui::SetNextWindowViewport(viewport->ID);

            SetWindowStyle();
            ImGui::Begin(NAME, &UIUtil::OPEN, FLAGS);
        }

        windowId = ImGui::GetID(NAME);

        ImGui::PopStyleVar(3);

        context->getEditorContext().dockService.buildViews(windowId, this);

        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
        ImGui::DockSpace(windowId, CENTER, ImGuiDockNodeFlags_PassthruCentralNode);
        ImGui::PopStyleVar(1);

        onSyncChildren();
        ImGui::End();
    }

    void EditorPanel::renderHeader(ImGuiViewport *viewport) {
        UIUtil::AUX_VEC2.x = viewport->Pos.x;
        UIUtil::AUX_VEC2.y = 0;
        ImGui::SetNextWindowPos(UIUtil::AUX_VEC2);

        UIUtil::AUX_VEC2.x = viewport->Size.x;
        UIUtil::AUX_VEC2.y = HEADER_HEIGHT;
        ImGui::SetNextWindowSize(UIUtil::AUX_VEC2);

        SetWindowStyle();
        ImGui::Begin(NAME_HEADER, &Metal::UIUtil::OPEN, FLAGS | ImGuiWindowFlags_NoScrollbar);
        ImGui::PopStyleVar(3);

        headerPanel.onSync();
        ImGui::End();
    }

    void EditorPanel::SetWindowStyle() {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, UIUtil::VEC2_ZERO);
    }

    void EditorPanel::onSync() {
        renderDockSpaces();
    }

    void EditorPanel::onInitialize() {
        headerPanel.setContext(context);
        headerPanel.onInitialize();
    }
}