#include "NavigationPanel.h"

#include <imgui.h>

#include "NavigationButtonsPanel.h"
#include "../../../MetalContextProvider.h"
#include "../../data/Views.h"
#include "../../util/UIUtil.h"

#define BUTTONS_SIZE 30
#define MARGIN 8

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
        std::unordered_map<NavigationPosition, unsigned int> &selected = SINGLETONS.editorRepository.viewsSelected;
        if (!selected.contains(position)) {
            selected.emplace(position, getDefaultView());
        }

        if (selected.at(position) != selectedLocal) {
            if (selectedView != nullptr) {
                removeChild(selectedView);
            }
            appendChild(selectedView = Views::OPTIONS.at(selected.at(position)).getPanel());
            selectedLocal = selected.at(position);
        }

        switch (position) {
            case NavigationPosition::BOTTOM: {
                ImGui::SetNextWindowSize(ImVec2(BUTTONS_SIZE, ImGui::GetContentRegionAvail().y));

                buttonsPanel->onSync();
                ImGui::SameLine();

                UIUtil::BeginEmptyWindow((id + "window").c_str(),
                              ImVec2(ImGui::GetContentRegionAvail().x,
                                     ImGui::GetContentRegionAvail().y));
                selectedView->onSync();
                UIUtil::EndEmptyWindow();
                break;
            }
            case NavigationPosition::LEFT: {
                ImGui::SetNextWindowSize(ImVec2(BUTTONS_SIZE, ImGui::GetContentRegionAvail().y));

                buttonsPanel->onSync();
                ImGui::SameLine();
                UIUtil::BeginEmptyWindow((id + "window").c_str(),
                                       ImVec2(ImGui::GetContentRegionAvail().x - MARGIN,
                                              ImGui::GetContentRegionAvail().y));
                selectedView->onSync();
                UIUtil::EndEmptyWindow();

                break;
            }
            case NavigationPosition::RIGHT: {
                UIUtil::BeginEmptyWindow((id + "window").c_str(), ImVec2(ImGui::GetWindowWidth() - BUTTONS_SIZE - MARGIN,
                                                                       ImGui::GetContentRegionAvail().y));
                selectedView->onSync();
                UIUtil::EndEmptyWindow();

                ImGui::SameLine();
                ImGui::SetNextWindowSize(ImVec2(BUTTONS_SIZE, ImGui::GetContentRegionAvail().y));
                buttonsPanel->onSync();
                break;
            }
        }
    }
} // Metal
