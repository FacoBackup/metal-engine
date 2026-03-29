#include "CameraGizmoPanel.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <ImGuizmo.h>
#include <vector>
#include <glm/vec3.hpp>
#include "engine/repository/WorldRepository.h"
#include "ApplicationEventContext.h"
#include "editor/ui/UIUtil.h"
#include "common/Icons.h"
#include "ViewportHeaderPanel.h"

namespace Metal {
    void CameraGizmoPanel::onSync() {
        auto &camera = worldRepository->camera;

        ImGuizmo::SetDrawlist();

        ImVec2 viewportPos = ImGui::GetWindowPos();
        ImVec2 viewportSize = ImGui::GetWindowSize();
        const float headerOffset = ViewportHeaderPanel::HEIGHT;

        ImGuizmo::SetRect(viewportPos.x, viewportPos.y + headerOffset, viewportSize.x, viewportSize.y - headerOffset);

        ImVec2 gizmoPos = ImVec2(viewportPos.x + viewportSize.x - GIZMO_SIZE - 10, viewportPos.y + headerOffset + 10);

        if (tempView != camera.viewMatrix && !isManipulating) {
            tempView = camera.viewMatrix;
        }

        ImGuizmo::ViewManipulate(glm::value_ptr(tempView), 8.0f, gizmoPos, ImVec2(GIZMO_SIZE, GIZMO_SIZE),
                                 IM_COL32(0, 0, 0, 0));
        ImGuizmo::SetRect(viewportPos.x, viewportPos.y, viewportSize.x, viewportSize.y);
        ImGui::SetCursorPos(ImVec2(viewportSize.x - GIZMO_SIZE - 10, headerOffset + 10));
        ImGui::InvisibleButton("gizmoButton", ImVec2(GIZMO_SIZE, GIZMO_SIZE));

        isManipulating = camera.viewMatrix != tempView && ImGui::IsItemHovered();
        if (isManipulating) {
            camera.viewMatrix = tempView;
            glm::mat4 invView = glm::inverse(tempView);
            glm::vec3 forward = glm::normalize(invView[2]); // world-space forward

            camera.yaw = atan2(forward.x, forward.z);
            camera.pitch = asin(forward.y);

            ApplicationEventContext::dispatch("Camera");
        }

        // ImGui::SetCursorPos(ImVec2(viewportSize.x - GIZMO_SIZE / 2.0f - 25, GIZMO_SIZE + headerOffset + 15));
        // const std::string icon = camera.isOrthographic ? Icons::videocam_off : Icons::videocam;
        // if (UIUtil::ButtonSimple(icon + id + "projectionMode", 30, 30)) {
        //     camera.isOrthographic = !camera.isOrthographic;
        //     ApplicationEventContext::dispatch("Camera");
        // }
        // UIUtil::RenderTooltip(camera.isOrthographic ? "Switch to Perspective" : "Switch to Orthographic");
    }
} // Metal
