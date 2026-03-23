#include "EditorPanel.h"
#include "editor/ui/UIUtil.h"
#include "ApplicationContext.h"
#include "../service/DockService.h"
#include "../service/ThemeService.h"
#include "../repository/EditorRepository.h"
#include "header/EditorHeaderPanel.h"
#include "footer/EditorFooterPanel.h"
#include "NotificationsPanel.h"

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
    float EditorPanel::HEADER_HEIGHT = 28;
    float EditorPanel::FOOTER_HEIGHT = 30;


    void EditorPanel::renderDockSpaces() {
        const ImGuiViewport *viewport = ImGui::GetMainViewport();

        ImU32 accentColorRaw = editorRepository->accentU32;
        ImU32 accentColor = (accentColorRaw & 0x00FFFFFF) | (0x60000000);
        ImU32 transparentColor = accentColorRaw & 0x00FFFFFF;
        ImVec2 gradientSize(viewport->Size.x * 0.35f, viewport->Size.y * 0.15f);

        // Header
        {
            ImGui::SetNextWindowPos(ImVec2(
                viewport->Pos.x,
                viewport->Pos.y));

            ImGui::SetNextWindowSize(ImVec2(
                viewport->Size.x,
                HEADER_HEIGHT));

            SetWindowStyle(UIUtil::VEC2_ZERO);
            ImGui::Begin(NAME_HEADER, &UIUtil::OPEN, FLAGS | ImGuiWindowFlags_NoScrollbar);
            ImGui::PopStyleVar(3);

            ImGui::GetWindowDrawList()->AddRectFilledMultiColor(
                viewport->Pos,
                ImVec2(viewport->Pos.x + gradientSize.x, viewport->Pos.y + gradientSize.y),
                accentColor,
                transparentColor,
                transparentColor,
                transparentColor
            );

            headerPanel->onSync();
            ImGui::End();
        }
        ImVec2 tempVec2{};

        // Main Window (DockSpace)
        {
            tempVec2.x = viewport->Pos.x;
            tempVec2.y = viewport->Pos.y + HEADER_HEIGHT;
            ImGui::SetNextWindowPos(tempVec2);

            tempVec2.x = viewport->Size.x;
            tempVec2.y = viewport->Size.y - HEADER_HEIGHT - FOOTER_HEIGHT;
            ImGui::SetNextWindowSize(tempVec2);
            ImGui::SetNextWindowViewport(viewport->ID);

            tempVec2.x = 4.0f;
            tempVec2.y = .0f;
            SetWindowStyle(tempVec2);
            ImGui::Begin(NAME, &UIUtil::OPEN, FLAGS);
            windowId = ImGui::GetID(NAME);
            ImGui::PopStyleVar(3);

            ImGui::GetWindowDrawList()->AddRectFilledMultiColor(
                viewport->Pos,
                ImVec2(viewport->Pos.x + gradientSize.x, viewport->Pos.y + gradientSize.y),
                accentColor,
                transparentColor,
                transparentColor,
                transparentColor
            );

            dockService->buildViews(windowId, this);

            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
            ImGui::DockSpace(windowId, CENTER, ImGuiDockNodeFlags_PassthruCentralNode);
            ImGui::PopStyleVar(1);

            onSyncChildren();
            ImGui::End();
        }

        // Footer
        {
            tempVec2.x = viewport->Pos.x;
            tempVec2.y = viewport->Pos.y + viewport->Size.y - FOOTER_HEIGHT;
            ImGui::SetNextWindowPos(tempVec2);

            tempVec2.x = viewport->Size.x;
            tempVec2.y = FOOTER_HEIGHT;
            ImGui::SetNextWindowSize(tempVec2);

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
    }

    void EditorPanel::onInitialize() {
        headerPanel = initializePanel<EditorHeaderPanel>(false);
        footerPanel = initializePanel<EditorFooterPanel>(false);
        notificationsPanel = initializePanel<NotificationsPanel>(false);
    }
}
