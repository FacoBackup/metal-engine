#include "NavigationPanel.h"

#include <imgui.h>

#include "NavigationButtonsPanel.h"
#include "../../../MetalContextProvider.h"
#include "../../data/Views.h"

#define WINDOW_FLAGS ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse

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

        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.f);
        switch (position) {
            case NavigationPosition::BOTTOM: {
                if (ImGui::BeginChild((id + "window").c_str())) {
                    selectedView->onSync();
                }
                ImGui::EndChild();
                ImGui::NewLine();

                buttonsPanel->onSync();
                break;
            }
            case NavigationPosition::LEFT: {
                buttonsPanel->onSync();
                ImGui::SameLine();
                if (ImGui::BeginChild((id + "window").c_str())) {
                    selectedView->onSync();
                }
                ImGui::EndChild();
                break;
            }
            case NavigationPosition::RIGHT: {
                if (ImGui::BeginChild((id + "window").c_str())) {
                    selectedView->onSync();
                }
                ImGui::EndChild();
                ImGui::SameLine();
                buttonsPanel->onSync();
                break;
            }
        }
        ImGui::PopStyleVar();
    }
} // Metal
