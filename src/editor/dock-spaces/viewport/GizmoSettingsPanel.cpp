#include "GizmoSettingsPanel.h"
#include "../../util/UIUtil.h"
#include "../../../../context/ApplicationContext.h"
#include "ImGuizmo.h"

namespace Metal {
    void GizmoSettingsPanel::onInitialize() {
        editorRepository = &CTX.editorRepository;
    }

    void GizmoSettingsPanel::onSync() {
        gizmoMode();
        ImGui::SameLine();
        gizmoSelection();
        ImGui::SameLine();
        gizmoGrid();
        UIUtil::Spacing();
        if (selectedEntityId != editorRepository->mainSelection && CTX.worldRepository.registry.all_of(
                editorRepository->mainSelection)) {
            selectedEntity = CTX.worldRepository.getEntity(editorRepository->mainSelection);
            selectedEntityId = editorRepository->mainSelection;
        }
        if (selectedEntity != nullptr) {
            ImGui::Text(selectedEntity->name.c_str());
        }
    }

    void GizmoSettingsPanel::gizmoGrid() const {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, SPACING);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, SPACING);
        renderSnapTranslate();
        ImGui::PopStyleVar(2);

        UIUtil::Spacing();
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, SPACING);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, SPACING);
        renderSnapRotate();
        ImGui::PopStyleVar(2);

        UIUtil::Spacing();
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, SPACING);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, SPACING);
        renderSnapScale();
        ImGui::PopStyleVar(2);
    }

    void GizmoSettingsPanel::gizmoMode() const {
        if (ImGui::Button((editorRepository->gizmoMode == ImGuizmo::MODE::WORLD
                               ? Icons::language
                               : Icons::place).c_str())) {
            editorRepository->gizmoMode = editorRepository->gizmoMode == ImGuizmo::MODE::LOCAL
                                              ? ImGuizmo::MODE::WORLD
                                              : ImGuizmo::MODE::LOCAL;
        }
        UIUtil::RenderTooltip(editorRepository->gizmoMode == ImGuizmo::MODE::WORLD
                                  ? "Transform mode: World"
                                  : "Transform mode: Local");
    }

    void GizmoSettingsPanel::gizmoSelection() const {
        if (UIUtil::RenderOption(Icons::control_camera + id + "translate",
                                 editorRepository->gizmoType == ImGuizmo::OPERATION::TRANSLATE,
                                 true, editorRepository->accent)) {
            editorRepository->gizmoType = ImGuizmo::OPERATION::TRANSLATE;
        }
        UIUtil::RenderTooltip("Translate");
        ImGui::SameLine();
        if (UIUtil::RenderOption(Icons::crop_rotate + id + "rotate",
                                 editorRepository->gizmoType == ImGuizmo::OPERATION::ROTATE,
                                 true,
                                 editorRepository->accent)) {
            editorRepository->gizmoType = ImGuizmo::OPERATION::ROTATE;
        }
        UIUtil::RenderTooltip("Rotate");

        ImGui::SameLine();
        if (UIUtil::RenderOption(Icons::expand + id + "scaleGizmo",
                                 editorRepository->gizmoType == ImGuizmo::OPERATION::SCALE,
                                 true,
                                 editorRepository->accent)) {
            editorRepository->gizmoType = ImGuizmo::OPERATION::SCALE;
        }
        UIUtil::RenderTooltip("Scale");
    }

    void GizmoSettingsPanel::renderSnapTranslate() const {
        if (UIUtil::RenderOption(Icons::control_camera + id + "translateSnap", editorRepository->gizmoUseSnapTranslate,
                                 true,
                                 editorRepository->accent)) {
            editorRepository->gizmoUseSnapTranslate = !editorRepository->gizmoUseSnapTranslate;
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(50);
        if (ImGui::Combo((id + "translateSnapAngle").c_str(), &editorRepository->gizmoSnapTranslateOption,
                         SNAP_TRANSLATE_OPTIONS)) {
            editorRepository->gizmoSnapTranslate = SNAP_TRANSLATE_OPTIONS_A[editorRepository->gizmoSnapTranslateOption];
        }
    }

    void GizmoSettingsPanel::renderSnapRotate() const {
        if (UIUtil::RenderOption(Icons::i360 + id + "rotateSnap", editorRepository->gizmoUseSnapRotate, true,
                                 editorRepository->accent)) {
            editorRepository->gizmoUseSnapRotate = !editorRepository->gizmoUseSnapRotate;
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(50);
        if (ImGui::Combo((id + "rotateSnapAngle").c_str(), &editorRepository->gizmoSnapRotateOption,
                         SNAP_ROTATE_OPTIONS)) {
            editorRepository->gizmoSnapRotate = SNAP_ROTATE_OPTIONS_A[editorRepository->gizmoSnapRotateOption];
        }
    }

    void GizmoSettingsPanel::renderSnapScale() const {
        if (UIUtil::RenderOption(Icons::expand + id + "scaleSnap", editorRepository->gizmoUseSnapScale, true,
                                 editorRepository->accent)) {
            editorRepository->gizmoUseSnapScale = !editorRepository->gizmoUseSnapScale;
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(50);
        if (ImGui::Combo((id + "scaleSnapAngle").c_str(), &editorRepository->gizmoSnapScaleOption, SNAP_SCALE_OPTIONS,
                         ImGuiComboFlags_NoArrowButton)) {
            editorRepository->gizmoSnapScale = SNAP_SCALE_OPTIONS[editorRepository->gizmoSnapScaleOption];
        }
    }
} // Metal
