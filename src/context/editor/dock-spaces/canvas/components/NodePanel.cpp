#include "NodePanel.h"

#include "IOPanel.h"
#include "../CanvasUtil.h"
#include "../../../../../dto/scripting/abstract/AbstractDraggable.h"
#include "../../../../../dto/scripting/ScriptInstance.h"

namespace Metal {
    void NodePanel::onSync() {
        auto pos = ImGui::GetWindowPos();

        node->nodeWindowPos.x = pos.x + node->x;
        node->nodeWindowPos.y = pos.y + node->y + 50;
        ImVec2 nodeSize = ImVec2(node->width, node->height);
        ImVec2 nodeRectMin = node->nodeWindowPos;
        ImVec2 nodeRectMax = ImVec2(node->nodeWindowPos.x + nodeSize.x, node->nodeWindowPos.y + nodeSize.y);

        // Get current window's draw list.
        ImDrawList *drawList = ImGui::GetWindowDrawList();

        // --- Draw Shadow ---
        ImVec2 shadowOffset(5, 5);
        ImVec2 shadowMin = ImVec2(nodeRectMin.x + shadowOffset.x, nodeRectMin.y + shadowOffset.y);
        ImVec2 shadowMax = ImVec2(nodeRectMax.x + shadowOffset.x, nodeRectMax.y + shadowOffset.y);
        drawList->AddRectFilled(shadowMin, shadowMax, IM_COL32(0, 0, 0, 100), NODE_BORDER_RADIUS);

        // --- Draw Node Background ---
        drawList->AddRectFilled(nodeRectMin, nodeRectMax, IM_COL32(60, 60, 60, 255), NODE_BORDER_RADIUS);
        drawList->AddRect(nodeRectMin, nodeRectMax, IM_COL32(80, 80, 80, 255), NODE_BORDER_RADIUS, 0, 2.0f);

        ImVec2 headerRectMin = nodeRectMin;
        ImVec2 headerRectMax = ImVec2(nodeRectMax.x, nodeRectMin.y + NODE_HEADER_SIZE);
        // Compute text position (with an inset) without using operator+.
        ImVec2 textPos = ImVec2(headerRectMin.x + 10, headerRectMin.y + 5);
        bool isSelected = scriptInstance->selectedNodes.contains(node->id);
        if (isSelected) {
            drawList->AddText(textPos, IM_COL32(0, 128, 255, 255), node->name.c_str());
        } else {
            drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), node->name.c_str());
        }
        // Draw separator under the header.
        ImVec2 separatorStart = ImVec2(nodeRectMin.x, headerRectMax.y);
        ImVec2 separatorEnd = ImVec2(nodeRectMax.x, headerRectMax.y);
        drawList->AddLine(separatorStart, separatorEnd, IM_COL32(80, 80, 80, 255), 1.0f);

        // --- Handle Dragging on Header ---
        // Create an invisible button covering the header area.
        ImGui::SetCursorScreenPos(headerRectMin);
        std::string headerId = node->id + "_header";
        ImGui::InvisibleButton(headerId.c_str(), ImVec2(nodeSize.x, NODE_HEADER_SIZE));
        if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
            ImVec2 delta = ImGui::GetIO().MouseDelta;
            node->x += delta.x;
            node->y += delta.y;
        }
    }
} // Metal
