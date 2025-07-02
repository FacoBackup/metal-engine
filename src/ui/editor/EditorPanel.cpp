#include "EditorPanel.h"

#include "dock-spaces/notifications/NotificationPanel.h"
#include "../util/UIUtil.h"
#include "../../MetalContextProvider.h"
#include "basic/NavigationPanel.h"
#include "dock-spaces/header/EditorHeaderPanel.h"
#include "dock-spaces/header/GlobalSettingsPanel.h"
#include "dock-spaces/viewport/ViewportPanel.h"

#define MARGIN 1.0f
#define WINDOW_FLAGS ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground |ImGuiWindowFlags_NoScrollbar
#define TOP_BAR_OFFSET 50

namespace Metal {
    void EditorPanel::renderLeftColumn() {
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        float viewSize = viewport->Size.y * SINGLETONS.editorRepository.topBlockRatio - MARGIN - TOP_BAR_OFFSET;

        ImGui::SetNextWindowPos(ImVec2(MARGIN, TOP_BAR_OFFSET), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(SINGLETONS.editorRepository.leftBlockSize, viewSize), ImGuiCond_Always);
        if (ImGui::Begin((id + "left").c_str(), nullptr, WINDOW_FLAGS)) {
            if (ImGui::BeginChild((id + "headerLeft").c_str(),
                                  ImVec2(SINGLETONS.editorRepository.leftBlockSize, TOP_BAR_OFFSET),
                                  UIUtil::OPEN)) {
                pLeftHeader->onSync();
            }
            ImGui::EndChild();
            pLeftNavigation->onSync();
        }
        ImGui::End();
    }

    void EditorPanel::renderRightColumn() {
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        float viewSize = viewport->Size.y * SINGLETONS.editorRepository.topBlockRatio - MARGIN * 2;

        ImGui::SetNextWindowPos(ImVec2(viewport->Size.x - SINGLETONS.editorRepository.rightBlockSize - MARGIN,
                                       MARGIN),
                                ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(SINGLETONS.editorRepository.rightBlockSize, viewSize), ImGuiCond_Always);
        if (ImGui::Begin((id + "right").c_str(), nullptr, WINDOW_FLAGS)) {
            if (ImGui::BeginChild((id + "headerRight").c_str(),
                                  ImVec2(SINGLETONS.editorRepository.rightBlockSize, TOP_BAR_OFFSET),
                                  UIUtil::OPEN)) {
                pRightHeader->onSync();
            }
            ImGui::EndChild();
            pRightNavigation->onSync();
        }
        ImGui::End();
    }

    void EditorPanel::renderBottomColumn() {
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        float viewPosition = viewport->Size.y * SINGLETONS.editorRepository.topBlockRatio;
        float viewSize = viewport->Size.y * (1. - SINGLETONS.editorRepository.topBlockRatio) - MARGIN;
        ImGui::SetNextWindowPos(ImVec2(MARGIN, viewPosition), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(viewport->Size.x - MARGIN - MARGIN, viewSize), ImGuiCond_Always);
        if (ImGui::Begin((id + "bottom").c_str(), nullptr, WINDOW_FLAGS)) {
            pBottomNavigation->onSync();
        }
        ImGui::End();
    }

    void EditorPanel::renderTopColumn() {
        // const ImGuiViewport *viewport = ImGui::GetMainViewport();
        // ImGui::SetNextWindowPos(ImVec2(MARGIN, MARGIN), ImGuiCond_Always);
        // ImGui::SetNextWindowSize(ImVec2(viewport->Size.x - MARGIN - MARGIN, TOP_BAR_OFFSET), ImGuiCond_Always);
        // if (ImGui::Begin((id + "top").c_str(), nullptr, WINDOW_FLAGS)) {
        //     if (ImGui::BeginChild((id + "topWindow").c_str())) {
        //         pHeader->onSync();
        //     }
        //     ImGui::EndChild();
        // }
        // ImGui::End();
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

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.f);
        renderTopColumn();
        renderLeftColumn();
        renderRightColumn();
        renderBottomColumn();
        ImGui::PopStyleVar();

        pNotifications->onSync();
    }

    void EditorPanel::onInitialize() {
        appendChild(pLeftNavigation = new NavigationPanel(LEFT));
        appendChild(pRightNavigation = new NavigationPanel(RIGHT));
        appendChild(pBottomNavigation = new NavigationPanel(BOTTOM));
        appendChild(pViewport = new ViewportPanel);
        appendChild(pNotifications = new NotificationPanel);
        appendChild(pLeftHeader = new EditorHeaderPanel);
        appendChild(pRightHeader = new GlobalSettingsPanel);
    }
}
