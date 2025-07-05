#include "NavigationPanel.h"

#include <imgui.h>

#include "NavigationButtonsPanel.h"
#include "../../../MetalContextProvider.h"
#include "../../data/Views.h"
#include "../../util/UIUtil.h"
#define RESIZE_BAR_SIZE 4

namespace Metal {
    void NavigationPanel::onInitialize() {
        appendChild(buttonsPanel = new NavigationButtonsPanel(position));
    }

    unsigned int NavigationPanel::getDefaultView() {
        switch (position) {
            case NavigationPosition::LEFT:
                return Views::FindIndexByName(Views::HIERARCHY);
            case NavigationPosition::RIGHT:
                return Views::FindIndexByName(Views::INSPECTOR);
            case NavigationPosition::BOTTOM:
                return Views::FindIndexByName(Views::FILES);
        }
        return 0;
    }

    void NavigationPanel::onSync() {
        std::array<ViewInstance, 2> &selected = SINGLETONS.editorRepository.views.at(position);

        for (int i = 0; i < 2; i++) {
            PanelInstance &panel = panels.at(i);
            ViewInstance &instance = selected.at(i);
            if (instance.initialized == false) {
                continue;
            }
            if (panel.panel == nullptr) {
                panel.instance = &instance;
                panel.panel = appendChild(Views::OPTIONS[instance.viewIndex].getPanel());
            }
            switch (position) {
                case NavigationPosition::BOTTOM: {
                    float viewWidth = ImGui::GetContentRegionAvail().x * instance.viewRatio;
                    // ImGui::SetNextWindowSize(ImVec2(BUTTONS_SIZE, ImGui::GetContentRegionAvail().y));
                    //
                    // buttonsPanel->onSync();
                    // ImGui::SameLine();
                    UIUtil::BeginEmptyWindow((id + "window").c_str(),
                                             ImVec2(viewWidth,
                                                    ImGui::GetContentRegionAvail().y));
                    panel.panel->onSync();
                    UIUtil::EndEmptyWindow();
                    ImGui::SameLine();
                    break;
                }
                case NavigationPosition::LEFT: {
                    // ImGui::SetNextWindowSize(ImVec2(BUTTONS_SIZE, ImGui::GetContentRegionAvail().y));
                    //
                    // buttonsPanel->onSync();
                    // ImGui::SameLine();
                    UIUtil::BeginEmptyWindow((id + "window").c_str(),
                                             ImVec2(ImGui::GetContentRegionAvail().x,
                                                    ImGui::GetContentRegionAvail().y * instance.viewRatio));
                    // if (i == 0 && selected.at(1).initialized) {
                    //     ImGui::InvisibleButton((id + "v_splitterLeft").c_str(), ImVec2(viewWidth,  RESIZE_BAR_SIZE));
                    //     if (ImGui::IsItemHovered()) {
                    //         ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
                    //     }
                    //     if (ImGui::IsItemActive()) {
                    //         SINGLETONS.editorRepository.topBlockRatio += ImGui::GetIO().MouseDelta.y/viewport->Size.y;
                    //     }
                    //     ImGui::NewLine();
                    // }
                    panel.panel->onSync();
                    UIUtil::EndEmptyWindow();

                    break;
                }
                case NavigationPosition::RIGHT: {
                    UIUtil::BeginEmptyWindow((id + "window").c_str(), ImVec2(
                                                 ImGui::GetWindowWidth(),
                                                 ImGui::GetContentRegionAvail().y * instance.viewRatio));

                    panel.panel->onSync();
                    UIUtil::EndEmptyWindow();

                    // ImGui::SameLine();
                    // ImGui::SetNextWindowSize(ImVec2(BUTTONS_SIZE, ImGui::GetContentRegionAvail().y));
                    // buttonsPanel->onSync();
                    break;
                }
            }
        }
    }
} // Metal
