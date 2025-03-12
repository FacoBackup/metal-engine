#include "IOPanel.h"

#include <imgui.h>

#include "../../../../../dto/scripting/abstract/AbstractDraggable.h"
#include "../../../../../dto/scripting/ScriptInstance.h"
#include "../CanvasUtil.h"

namespace Metal {
    void IOPanel::onSync() {
        for (const std::string& ioId: node->ioList) {
            auto &io = node->ioMap.at(ioId);
            if (io.isInput) {
                io.circlePos = ImVec2(node->nodeWindowPos.x + IO_CIRCLE_RADIUS_2,
                                       node->nodeWindowPos.y + io.offsetY * IO_CIRCLE_RADIUS_3 + NODE_HEADER_SIZE +
                                       IO_CIRCLE_RADIUS_2);
            } else {
                io.circlePos = ImVec2(node->nodeWindowPos.x + node->width - IO_CIRCLE_RADIUS_2,
                                       node->nodeWindowPos.y + io.offsetY * IO_CIRCLE_RADIUS_3 + NODE_HEADER_SIZE +
                                       IO_CIRCLE_RADIUS_2);
            }

            char buf[64];
            snprintf(buf, sizeof(buf), "io_%s_%s", io.id.c_str(), io.isInput ? "in" : "out");
            ImGui::SetCursorScreenPos(ImVec2(io.circlePos.x - IO_CIRCLE_RADIUS, io.circlePos.y - IO_CIRCLE_RADIUS));
            ImGui::InvisibleButton(buf, ImVec2(IO_CIRCLE_RADIUS_2, IO_CIRCLE_RADIUS_2));
            bool ioHovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);

            if (io.isInput && ioHovered && draggingOutput) {
                hoveredInput = &io;
            }

            ImU32 ioColor = CanvasUtil::GetColorForIOType(io.type);
            if (io.isInput && draggingOutput && ioHovered) {
                ioColor = IM_COL32(200, 200, 255, 255);
            }

            ImGui::GetWindowDrawList()->AddCircleFilled(io.circlePos, IO_CIRCLE_RADIUS, ioColor);
            if (io.name != nullptr && strlen(io.name) > 0) {
                auto size = ImGui::CalcTextSize(io.name);

                if (io.isInput) {
                    ImGui::SetCursorScreenPos(ImVec2(io.circlePos.x + IO_CIRCLE_RADIUS_2,
                                                     io.circlePos.y - size.y / 2.f));
                } else {
                    ImGui::SetCursorScreenPos(ImVec2(io.circlePos.x - IO_CIRCLE_RADIUS_2 - size.x,
                                                     io.circlePos.y - size.y / 2.f));
                }
                ImGui::Text(io.name);
            }
            if (!io.isInput) {
                if (!draggingOutput && ioHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    draggingOutput = &io;
                    draggingOrigin = io.circlePos;
                }
            }
        }
    }

    void IOPanel::onAfterSync() {
        if (draggingOutput) {
            ImVec2 mousePos = ImGui::GetIO().MousePos;
            ImGui::GetWindowDrawList()->AddBezierCubic(
                draggingOrigin,
                ImVec2(draggingOrigin.x + 50, draggingOrigin.y),
                ImVec2(mousePos.x - 50, mousePos.y),
                mousePos,
                CanvasUtil::GetColorForIOType(draggingOutput->type),
                2.0f);

            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                if (hoveredInput) {
                    scriptInstance->addLink(*draggingOutput, *hoveredInput);
                }
                draggingOutput = nullptr;
                hoveredInput = nullptr;
            }
        }
    }
} // Metal
