#include "WorldHeaderPanel.h"
#include "ApplicationContext.h"
#include "editor/ui/UIUtil.h"
#include "engine/enum/ComponentType.h"
#include "engine/repository/WorldRepository.h"
#include "common/Icons.h"

namespace Metal {
    void WorldHeaderPanel::onSync() {
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 120.f - UIUtil::ONLY_ICON_BUTTON_SIZE - 20.f);
        ImGui::InputTextWithHint(("##hierarchySearch" + id).c_str(), "Search...", search, sizeof(search));
        ImGui::SameLine();
        ImGui::SetNextItemWidth(120.f);
        if (ImGui::BeginCombo(("##hierarchyFilter" + id).c_str(),
                              selectedComponentType == -1
                                  ? "All"
                                  : ComponentTypes::NameOf(static_cast<ComponentType>(selectedComponentType)))) {
            if (ImGui::Selectable("All", selectedComponentType == -1)) {
                selectedComponentType = -1;
            }
            for (const auto &compDef: ComponentTypes::getComponents()) {
                if (ImGui::Selectable((compDef.icon + compDef.name).c_str(),
                                      selectedComponentType == static_cast<int>(compDef.type))) {
                    selectedComponentType = static_cast<int>(compDef.type);
                }
            }
            ImGui::EndCombo();
        }
        ImGui::SameLine();
        if (UIUtil::ButtonSimple(Icons::add + "##hierarchyAdd" + id, UIUtil::ONLY_ICON_BUTTON_SIZE,
                                 UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            worldRepository->createEntity();
        }
        UIUtil::RenderTooltip("Create entity");
    }
} // Metal
