#include "EditorPanel.h"

#include "dock-spaces/notifications/NotificationPanel.h"
#include "../util/UIUtil.h"
#include "../../MetalContextProvider.h"
#include "basic/NavigationPanel.h"
#include "dock-spaces/header/EditorHeaderPanel.h"
#include "dock-spaces/header/GlobalSettingsPanel.h"
#include "dock-spaces/viewport/ViewportPanel.h"

#define MARGIN 4.0f
#define WINDOW_FLAGS ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground |ImGuiWindowFlags_NoScrollbar
#define TOP_BAR_OFFSET 35
#define RESIZE_BAR_SIZE 4

namespace Metal {
    void EditorPanel::renderLeftColumn() {
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        float viewHeight = viewport->Size.y * SINGLETONS.editorRepository.topBlockRatio - MARGIN * 2 - TOP_BAR_OFFSET;

        UIUtil::BeginWindow(ImVec2(MARGIN, MARGIN * 2 + TOP_BAR_OFFSET),
                            ImVec2(SINGLETONS.editorRepository.leftBlockSize, viewHeight));
        if (ImGui::Begin((id + "left").c_str(), nullptr, WINDOW_FLAGS)) {
            pLeftNavigation->onSync();

            ImGui::SameLine();

            ImGui::InvisibleButton("v_splitterLeft", ImVec2(RESIZE_BAR_SIZE,  viewHeight));
            if (ImGui::IsItemHovered()) {
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
            }
            if (ImGui::IsItemActive()) {
                SINGLETONS.editorRepository.leftBlockSize += ImGui::GetIO().MouseDelta.x;
            }
        }
        ImGui::End();
    }

    void EditorPanel::renderRightColumn() {
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        float viewHeight = viewport->Size.y * SINGLETONS.editorRepository.topBlockRatio - MARGIN * 2 - TOP_BAR_OFFSET;

        UIUtil::BeginWindow(ImVec2(viewport->Size.x - SINGLETONS.editorRepository.rightBlockSize - MARGIN,
                                   MARGIN * 2 + TOP_BAR_OFFSET),
                            ImVec2(SINGLETONS.editorRepository.rightBlockSize, viewHeight));
        if (ImGui::Begin((id + "right").c_str(), nullptr, WINDOW_FLAGS)) {
            ImGui::InvisibleButton("v_splitterRight", ImVec2(RESIZE_BAR_SIZE,  viewHeight));
            if (ImGui::IsItemHovered()) {
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
            }
            if (ImGui::IsItemActive()) {
                SINGLETONS.editorRepository.rightBlockSize -= ImGui::GetIO().MouseDelta.x;
            }
            ImGui::SameLine();
            pRightNavigation->onSync();
        }
        ImGui::End();
    }

    void EditorPanel::renderBottomColumn() {
        const ImGuiViewport *viewport = ImGui::GetMainViewport();

        float viewY = viewport->Size.y * SINGLETONS.editorRepository.topBlockRatio + MARGIN;
        float viewHeight = viewport->Size.y * (1. - SINGLETONS.editorRepository.topBlockRatio) - MARGIN * 2;
        float viewWidth = viewport->Size.x - MARGIN * 2;

        UIUtil::BeginWindow(ImVec2(MARGIN, viewY), ImVec2(viewWidth, viewHeight));
        if (ImGui::Begin((id + "bottom").c_str(), nullptr, WINDOW_FLAGS)) {
            ImGui::InvisibleButton("v_splitterBottom", ImVec2(viewWidth,  RESIZE_BAR_SIZE));
            if (ImGui::IsItemHovered()) {
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
            }
            if (ImGui::IsItemActive()) {
                SINGLETONS.editorRepository.topBlockRatio += ImGui::GetIO().MouseDelta.y/viewport->Size.y;
            }
            ImGui::NewLine();
            pBottomNavigation->onSync();
        }
        ImGui::End();
    }

    void EditorPanel::renderTopColumn() {
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        UIUtil::BeginWindow(ImVec2(MARGIN, MARGIN), ImVec2(viewport->Size.x - MARGIN - MARGIN, TOP_BAR_OFFSET));
        if (ImGui::Begin((id + "top").c_str(), nullptr, WINDOW_FLAGS)) {
            pHeader->onSync();
        }
        ImGui::End();
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
        appendChild(pHeader = new EditorHeaderPanel);
    }
}
