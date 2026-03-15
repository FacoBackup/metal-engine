#include "EditorPanel.h"
#include "util/UIUtil.h"
#include "../ApplicationContext.h"
#include "service/DockService.h"
#include "service/ThemeService.h"
#include "dock-spaces/header/EditorHeaderPanel.h"
#include "dock-spaces/footer/EditorFooterPanel.h"
#include "panel/FileImportModalPanel.h"
#include "panel/NotificationsPanel.h"

namespace Metal {
    int EditorPanel::FLAGS = ImGuiWindowFlags_NoDocking |
                             ImGuiWindowFlags_NoTitleBar |
                             ImGuiWindowFlags_NoCollapse |
                             ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoBringToFrontOnFocus |
                             ImGuiWindowFlags_NoNavFocus;
    const char *EditorPanel::NAME = "##main_window";
    const char *EditorPanel::NAME_HEADER = "##header_window";
    const char *EditorPanel::NAME_FOOTER = "##footer_window";
    ImVec2 EditorPanel::CENTER(0.0f, 0.0f);
    float EditorPanel::HEADER_HEIGHT = 35;
    float EditorPanel::FOOTER_HEIGHT = 30;


    void EditorPanel::renderDockSpaces() {
        const ImGuiViewport *viewport = ImGui::GetMainViewport();

        // Header
        {
            UIUtil::AUX_VEC2.x = viewport->Pos.x;
            UIUtil::AUX_VEC2.y = viewport->Pos.y;
            ImGui::SetNextWindowPos(UIUtil::AUX_VEC2);

            UIUtil::AUX_VEC2.x = viewport->Size.x;
            UIUtil::AUX_VEC2.y = HEADER_HEIGHT;
            ImGui::SetNextWindowSize(UIUtil::AUX_VEC2);

            SetWindowStyle(UIUtil::VEC2_ZERO);
            ImGui::Begin(NAME_HEADER, &UIUtil::OPEN, FLAGS | ImGuiWindowFlags_NoScrollbar);
            ImGui::PopStyleVar(3);

            headerPanel->onSync();
            ImGui::End();
        }

        // Main Window (DockSpace)
        {
            UIUtil::AUX_VEC2.x = viewport->Pos.x;
            UIUtil::AUX_VEC2.y = viewport->Pos.y + HEADER_HEIGHT;
            ImGui::SetNextWindowPos(UIUtil::AUX_VEC2);

            UIUtil::AUX_VEC2.x = viewport->Size.x;
            UIUtil::AUX_VEC2.y = viewport->Size.y - HEADER_HEIGHT - FOOTER_HEIGHT;
            ImGui::SetNextWindowSize(UIUtil::AUX_VEC2);
            ImGui::SetNextWindowViewport(viewport->ID);

            UIUtil::AUX_VEC2.x = 4.0f;
            UIUtil::AUX_VEC2.y = .0f;
            SetWindowStyle(UIUtil::AUX_VEC2);
            ImGui::Begin(NAME, &UIUtil::OPEN, FLAGS);
            windowId = ImGui::GetID(NAME);
            ImGui::PopStyleVar(3);

            dockService->buildViews(windowId, this);

            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
            ImGui::DockSpace(windowId, CENTER, ImGuiDockNodeFlags_PassthruCentralNode);
            ImGui::PopStyleVar(1);

            onSyncChildren();
            ImGui::End();
        }

        // Footer
        {
            UIUtil::AUX_VEC2.x = viewport->Pos.x;
            UIUtil::AUX_VEC2.y = viewport->Pos.y + viewport->Size.y - FOOTER_HEIGHT;
            ImGui::SetNextWindowPos(UIUtil::AUX_VEC2);

            UIUtil::AUX_VEC2.x = viewport->Size.x;
            UIUtil::AUX_VEC2.y = FOOTER_HEIGHT;
            ImGui::SetNextWindowSize(UIUtil::AUX_VEC2);

            SetWindowStyle(UIUtil::VEC2_ZERO);
            ImGui::Begin(NAME_FOOTER, &UIUtil::OPEN, FLAGS | ImGuiWindowFlags_NoScrollbar);
            ImGui::PopStyleVar(3);

            footerPanel->onSync();
            ImGui::End();
        }
    }

    void EditorPanel::SetWindowStyle(const ImVec2 &padding) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, padding);
    }

    void EditorPanel::onSync() {
        themeService->onSync();
        renderDockSpaces();
        notificationsPanel->onSync();
        fileImportModalPanel->onSync();
    }

    void EditorPanel::onInitialize() {
        initializePanel(headerPanel = new EditorHeaderPanel(), false);
        initializePanel(footerPanel = new EditorFooterPanel(), false);
        initializePanel(notificationsPanel = new NotificationsPanel(), false);
        initializePanel(fileImportModalPanel = new FileImportModalPanel(), false);
    }
}
