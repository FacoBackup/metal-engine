#include "GizmoSettingsPanel.h"
#include "../../../common/util/UIUtil.h"
#include "../../../context/ApplicationContext.h"
#include "ImGuizmo.h"

namespace Metal {
    void GizmoSettingsPanel::onSync() {
        gizmoMode();
        gizmoSelection();
        gizmoGrid();
    }

    void GizmoSettingsPanel::onInitialize() {
        editorRepository = &context->getEditorContext().editorRepository;
    }

    void GizmoSettingsPanel::gizmoGrid() const {
        UIUtil::LargeSpacing();
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
        ImGui::SetNextItemWidth(85);
        if (ImGui::Button((editorRepository->gizmoMode == ImGuizmo::MODE::WORLD
                               ? Icons::language + "World"
                               : Icons::place + "Local").c_str())) {
            editorRepository->gizmoMode = editorRepository->gizmoMode == ImGuizmo::MODE::LOCAL
                                              ? ImGuizmo::MODE::WORLD
                                              : ImGuizmo::MODE::LOCAL;
        }
        UIUtil::LargeSpacing();
    }

    void GizmoSettingsPanel::gizmoSelection() const {
        if (UIUtil::RenderOption(Icons::control_camera + " Translate",
                                 editorRepository->gizmoType == ImGuizmo::OPERATION::TRANSLATE,
                                 false, editorRepository->accent)) {
            editorRepository->gizmoType = ImGuizmo::OPERATION::TRANSLATE;
        }
        ImGui::SameLine();
        if (UIUtil::RenderOption(Icons::crop_rotate + " Rotate",
                                 editorRepository->gizmoType == ImGuizmo::OPERATION::ROTATE, false,
                                 editorRepository->accent)) {
            editorRepository->gizmoType = ImGuizmo::OPERATION::ROTATE;
        }
        ImGui::SameLine();
        if (UIUtil::RenderOption(Icons::expand + " Scale", editorRepository->gizmoType == ImGuizmo::OPERATION::SCALE,
                                 false,
                                 editorRepository->accent)) {
            editorRepository->gizmoType = ImGuizmo::OPERATION::SCALE;
        }
    }

    void GizmoSettingsPanel::renderSnapTranslate() const {
        if (UIUtil::RenderOption(Icons::control_camera, editorRepository->gizmoUseSnapTranslate, true,
                                 editorRepository->accent)) {
            editorRepository->gizmoUseSnapTranslate = !editorRepository->gizmoUseSnapTranslate;
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(50);
        if (ImGui::Combo("##translateSnapAngle", &editorRepository->gizmoSnapTranslateOption, SNAP_TRANSLATE_OPTIONS)) {
            editorRepository->gizmoSnapTranslate = SNAP_TRANSLATE_OPTIONS_A[editorRepository->gizmoSnapTranslateOption];
        }
    }

    void GizmoSettingsPanel::renderSnapRotate() const {
        if (UIUtil::RenderOption(Icons::i360, editorRepository->gizmoUseSnapRotate, true, editorRepository->accent)) {
            editorRepository->gizmoUseSnapRotate = !editorRepository->gizmoUseSnapRotate;
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(50);
        if (ImGui::Combo("##rotateSnapAngle", &editorRepository->gizmoSnapRotateOption, SNAP_ROTATE_OPTIONS)) {
            editorRepository->gizmoSnapRotate = SNAP_ROTATE_OPTIONS_A[editorRepository->gizmoSnapRotateOption];
        }
    }

    void GizmoSettingsPanel::renderSnapScale() const {
        if (UIUtil::RenderOption(Icons::expand, editorRepository->gizmoUseSnapScale, true, editorRepository->accent)) {
            editorRepository->gizmoUseSnapScale = !editorRepository->gizmoUseSnapScale;
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(50);
        if (ImGui::Combo("##scaleSnapAngle", &editorRepository->gizmoSnapScaleOption, SNAP_SCALE_OPTIONS,
                         ImGuiComboFlags_NoArrowButton)) {
            editorRepository->gizmoSnapScale = SNAP_SCALE_OPTIONS[editorRepository->gizmoSnapScaleOption];
        }
    }
} // Metal
