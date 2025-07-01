#include "EditorPanel.h"

#include "dock-spaces/notifications/NotificationPanel.h"
#include "../util/UIUtil.h"
#include "../../MetalContextProvider.h"
#include "basic/NavigationPanel.h"
#include "dock-spaces/viewport/ViewportPanel.h"

#define MARGIN  8.0f
#define WINDOW_FLAGS ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse

namespace Metal {
    void EditorPanel::renderHorizontalColumns() {
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        float viewSize = viewport->Size.y * SINGLETONS.editorRepository.topBlockRatio;

        ImGui::SetNextWindowPos(ImVec2(MARGIN, MARGIN), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(SINGLETONS.editorRepository.leftBlockSize, viewSize), ImGuiCond_Always);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.f);
        if (ImGui::Begin((id + "left").c_str(), nullptr, WINDOW_FLAGS)) {
            leftNavigation->onSync();
        }
        ImGui::End();
        ImGui::PopStyleVar();


        ImGui::SetNextWindowPos(ImVec2(viewport->Size.x - SINGLETONS.editorRepository.rightBlockSize - MARGIN, MARGIN),
                                ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(SINGLETONS.editorRepository.rightBlockSize, viewSize), ImGuiCond_Always);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.f);
        if (ImGui::Begin((id + "right").c_str(), nullptr, WINDOW_FLAGS)) {
            rightNavigation->onSync();
        }
        ImGui::End();
        ImGui::PopStyleVar();
    }

    void EditorPanel::renderVerticalColumns() {
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        float viewPosition = viewport->Size.y * SINGLETONS.editorRepository.topBlockRatio;
        float viewSize = viewport->Size.y * (1. - SINGLETONS.editorRepository.topBlockRatio);
        ImGui::SetNextWindowPos(ImVec2(MARGIN, viewPosition - MARGIN), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(viewport->Size.x - MARGIN - MARGIN, viewSize - MARGIN), ImGuiCond_Always);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.f);
        if (ImGui::Begin((id + "bottom").c_str(), nullptr, WINDOW_FLAGS)) {
            bottomNavigation->onSync();
        }
        ImGui::End();
        ImGui::PopStyleVar();
    }

    void EditorPanel::onSync() {
        SINGLETONS.themeService.onSync();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::Begin("MainViewport", nullptr,
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar |
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus |
                     ImGuiWindowFlags_NoBackground);
        pViewport->onSync();
        ImGui::End();
        ImGui::PopStyleVar();

        renderHorizontalColumns();
        renderVerticalColumns();

        pNotifications->onSync();
    }

    void EditorPanel::onInitialize() {
        appendChild(leftNavigation = new NavigationPanel(LEFT));
        appendChild(rightNavigation = new NavigationPanel(RIGHT));
        appendChild(bottomNavigation = new NavigationPanel(BOTTOM));
        appendChild(pViewport = new ViewportPanel);
        appendChild(pNotifications = new NotificationPanel);
    }
}
