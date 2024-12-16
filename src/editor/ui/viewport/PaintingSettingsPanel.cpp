#include "PaintingSettingsPanel.h"

#include "../../../context/ApplicationContext.h"
#include "../../../common/util/UIUtil.h"
#include "../../../editor/EditorContext.h"

namespace Metal {
    void PaintingSettingsPanel::onSync() {
        auto &editorRepository = context->getEditorContext().editorRepository;
        auto addLabel = Icons::add + "Add";
        if (UIUtil::RenderOption(addLabel, editorRepository.brushModeAdd,
                                 ImGui::CalcTextSize(addLabel.c_str()).x + 8,
                                 25, editorRepository.accent)) {
            editorRepository.brushModeAdd = true;
        }
        UIUtil::Spacing();

        auto removeLabel = Icons::remove + "Remove";
        if (UIUtil::RenderOption(removeLabel, !editorRepository.brushModeAdd,
                                 ImGui::CalcTextSize(removeLabel.c_str()).x + 8,
                                 25, editorRepository.accent)) {
            editorRepository.brushModeAdd = false;
        }
        UIUtil::LargeSpacing();

        ImGui::SetNextItemWidth(150);
        ImGui::DragFloat("Brush radius", &editorRepository.brushRadius, .1f, 0, 250);
        UIUtil::Spacing();
        ImGui::SetNextItemWidth(150);
        ImGui::DragFloat("Brush density", &editorRepository.brushDensity, .001f, 0, 1);
    }
} // Metal
