#include "ViewportHeaderPanel.h"

#include "GizmoSettingsPanel.h"
#include "PaintingSettingsPanel.h"
#include "../docks/DockSpacePanel.h"
#include "../../../../util/UIUtil.h"
#include "../../../../context/ApplicationContext.h"

namespace Metal {
    void ViewportHeaderPanel::onInitialize() {
        appendChild(gizmo = new GizmoSettingsPanel());
        appendChild(paintGizmo = new PaintingSettingsPanel());
    }

    void ViewportHeaderPanel::onSync() {
        AbstractPanel *currentPanel;
        if (ApplicationContext::Get().editorRepository.editorMode == EditorMode::EditorMode::TRANSFORM) {
            currentPanel = gizmo;
        } else {
            currentPanel = paintGizmo;
        }

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, PADDING);
        if (ImGui::BeginChild(id.c_str(), ImVec2(0, HEIGHT), false, FLAGS)) {
            currentPanel->onSync();
        }
        ImGui::EndChild();
        ImGui::PopStyleVar();
    }
} // Metal
