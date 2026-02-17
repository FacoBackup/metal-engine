#include "ProjectInfoPanel.h"
#include "../../../../context/ApplicationContext.h"
#include "../../../../util/UIUtil.h"

namespace Metal {
    void ProjectInfoPanel::onSync() {
        ImGui::Text(Icons::inventory_2.c_str());
        ImGui::SameLine();

        if (strcmp(projectName, context->editorRepository.projectName.c_str()) != 0) {
            strcpy(projectName, context->editorRepository.projectName.c_str());
        }
        ImGui::SetNextItemWidth(150.0f);
        if (ImGui::InputText(id.c_str(), projectName, 128)) {
            context->editorRepository.projectName = projectName;
        }


        UIUtil::DynamicSpacing(225);
    }
}
