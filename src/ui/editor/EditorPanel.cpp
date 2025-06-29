#include "EditorPanel.h"
#include "../util/UIUtil.h"
#include "../../MetalContextProvider.h"
#include "../../ui/data/Notification.h"

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
    ImVec2 EditorPanel::CENTER(0.0f, 0.0f);
    float EditorPanel::HEADER_HEIGHT = 58;


    void EditorPanel::renderDockSpaces() {
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
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
        }

        ImGui::Begin(NAME, &UIUtil::OPEN, FLAGS);
        windowId = ImGui::GetID(NAME);

        ImGui::PopStyleVar(3);

        SINGLETONS.dockService.buildViews(windowId, this);

        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
        ImGui::DockSpace(windowId, CENTER, ImGuiDockNodeFlags_PassthruCentralNode);
        ImGui::PopStyleVar(1);

        onSyncChildren();
        ImGui::End();
    }

    void EditorPanel::renderHeader(const ImGuiViewport *viewport) {
        UIUtil::AUX_VEC2.x = viewport->Pos.x;
        UIUtil::AUX_VEC2.y = 0;
        ImGui::SetNextWindowPos(UIUtil::AUX_VEC2);

        UIUtil::AUX_VEC2.x = viewport->Size.x;
        UIUtil::AUX_VEC2.y = HEADER_HEIGHT;
        ImGui::SetNextWindowSize(UIUtil::AUX_VEC2);

        SetWindowStyle();
        ImGui::Begin(NAME_HEADER, &UIUtil::OPEN, FLAGS | ImGuiWindowFlags_NoScrollbar);
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
        SINGLETONS.themeService.onSync();
        renderDockSpaces();
        int usedIndices = 0;
        for (int i = 0; i < SINGLETONS.notificationService.getNotifications().size(); i++) {
            auto *notification = SINGLETONS.notificationService.getNotifications()[i];
            if (notification == nullptr) {
                continue;
            }
            if (notification->displayTime < 0) {
                notification->displayTime = SINGLETONS.engineContext.currentTimeMs;
            }
            if (SINGLETONS.engineContext.currentTimeMs - notification->displayTime > MESSAGE_DURATION) {
                delete notification;
                SINGLETONS.notificationService.getNotifications()[i] = nullptr;
                continue;
            }
            ImGui::SetNextWindowPos(ImVec2(5, ImGui::GetMainViewport()->Size.y - 40 * (usedIndices + 1)));
            ImGui::SetNextWindowSize(ImVec2(ImGui::CalcTextSize(notification->message.c_str()).x + 45, 35));
            ImGui::PushStyleColor(ImGuiCol_WindowBg, GetColor(notification->severity));

            ImGui::Begin("##toaster" + usedIndices, &UIUtil::OPEN,
                         ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
            ImGui::PopStyleColor();
            ImGui::TextColored(ImColor(255, 255, 255, 255), GetIcon(notification->severity));
            ImGui::SameLine();
            ImGui::TextColored(ImColor(255, 255, 255, 255), notification->message.c_str());
            ImGui::End();

            usedIndices++;
        }
    }

    void EditorPanel::onInitialize() {
        headerPanel.onInitialize();
    }
}
