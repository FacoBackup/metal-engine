#include "ViewportHeaderPanel.h"

#include "GizmoSettingsPanel.h"
#include "PaintingSettingsPanel.h"
#include "../docks/DockSpacePanel.h"
#include "../../../common/util/UIUtil.h"
#include "../../../context/ApplicationContext.h"

namespace Metal {
    void ViewportHeaderPanel::onInitialize() {
        appendChild(gizmo = new GizmoSettingsPanel());
        appendChild(paintGizmo = new PaintingSettingsPanel());
    }

    void ViewportHeaderPanel::onSync() {
        AbstractPanel *currentPanel;
        if (context->getEditorContext().editorRepository.editorMode == EditorMode::EditorMode::TRANSFORM) {
            currentPanel = gizmo;
        } else {
            currentPanel = paintGizmo;
        }
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y));
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetWindowSize().x, HEIGHT));
        ImGui::SetNextWindowBgAlpha(.4f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, PADDING);
        ImGui::Begin(id.c_str(), &UIUtil::OPEN, FLAGS);
        ImGui::PopStyleVar();
        currentPanel->onSync();
        ImGui::End();
    }
} // Metal
