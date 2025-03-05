#include "CanvasPanel.h"

#include "CanvasHeaderPanel.h"
#include "imgui.h"
#include "../../../ApplicationContext.h"
#include "../../../../dto/scripting/IO.h"
#define IO_CIRCLE_RADIUS 5.0f
#define IO_CIRCLE_RADIUS_2 IO_CIRCLE_RADIUS * 2.f
#define IO_CIRCLE_RADIUS_3 IO_CIRCLE_RADIUS * 3.f
#define NODE_HEADER_SIZE 25.f
#define NODE_BORDER_RADIUS 4.0f

namespace Metal {
    ImU32 GetColorForIOType(IOType type) {
        switch (type) {
            case IOType::IO_BOOLEAN: return IM_COL32(255, 100, 100, 255);
            case IOType::IO_FLOW: return IM_COL32(255, 255, 255, 255);
            // case IOType::String: return IM_COL32(100, 100, 255, 255);
            default: return IM_COL32(200, 200, 200, 255);
        }
    }

    static IO *s_draggingOutput = nullptr; // currently dragged output IO
    static ImVec2 s_draggingOrigin; // screen position of the dragging output circle
    static IO *s_hoveredInput = nullptr; // current drop target (input IO)

    void CanvasPanel::onInitialize() {
        appendChild(new CanvasHeaderPanel(&scriptInstance));
    }

    void CanvasPanel::RenderNodeIO(AbstractNode *nodeData, const ImVec2 &nodeWindowPos,
                                   ScriptInstance &scriptInstance) {
        for (auto &io: nodeData->io) {
            ImVec2 circlePos;
            if (io.isInput) {
                circlePos = ImVec2(nodeWindowPos.x + IO_CIRCLE_RADIUS_2,
                                   nodeWindowPos.y + io.offsetY * IO_CIRCLE_RADIUS_3 + NODE_HEADER_SIZE +
                                   IO_CIRCLE_RADIUS_2);
            } else {
                circlePos = ImVec2(nodeWindowPos.x + nodeData->width - IO_CIRCLE_RADIUS_2,
                                   nodeWindowPos.y + io.offsetY * IO_CIRCLE_RADIUS_3 + NODE_HEADER_SIZE +
                                   IO_CIRCLE_RADIUS_2);
            }

            char buf[64];
            snprintf(buf, sizeof(buf), "io_%s_%s", io.id.c_str(), io.isInput ? "in" : "out");
            ImGui::SetCursorScreenPos(ImVec2(circlePos.x - IO_CIRCLE_RADIUS, circlePos.y - IO_CIRCLE_RADIUS));
            ImGui::InvisibleButton(buf, ImVec2(IO_CIRCLE_RADIUS_2, IO_CIRCLE_RADIUS_2));
            bool ioHovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);

            if (io.isInput && ioHovered && s_draggingOutput) {
                s_hoveredInput = &io;
            }

            ImU32 ioColor = GetColorForIOType(io.type);
            if (io.isInput && s_draggingOutput && ioHovered) {
                ioColor = IM_COL32(200, 200, 255, 255);
            }

            ImGui::GetWindowDrawList()->AddCircleFilled(circlePos, IO_CIRCLE_RADIUS, ioColor);
            if (io.name != nullptr) {
                auto size = ImGui::CalcTextSize(io.name);

                if (io.isInput) {
                    ImGui::SetCursorScreenPos(ImVec2(circlePos.x + IO_CIRCLE_RADIUS_2, circlePos.y - size.y / 2.f));
                } else {
                    ImGui::SetCursorScreenPos(ImVec2(circlePos.x - IO_CIRCLE_RADIUS_2 - size.x,
                                                     circlePos.y - size.y / 2.f));
                }
                ImGui::Text(io.name);
            }
            if (!io.isInput) {
                if (!s_draggingOutput && ioHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    s_draggingOutput = &io;
                    s_draggingOrigin = circlePos;
                }
            }
        }

        if (s_draggingOutput) {
            ImVec2 mousePos = ImGui::GetIO().MousePos;
            ImGui::GetWindowDrawList()->AddBezierCubic(
                s_draggingOrigin,
                ImVec2(s_draggingOrigin.x + 50, s_draggingOrigin.y),
                ImVec2(mousePos.x - 50, mousePos.y),
                mousePos,
                GetColorForIOType(s_draggingOutput->type),
                2.0f);

            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                if (s_hoveredInput) {
                    scriptInstance.addLink(*s_draggingOutput, *s_hoveredInput);
                }
                s_draggingOutput = nullptr;
                s_hoveredInput = nullptr;
            }
        }
    }

    void CanvasPanel::handleNodeDrag(std::shared_ptr<AbstractNode> &nodeData) {
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered()) {
            if (ImGui::GetIO().KeyCtrl) {
                scriptInstance.selectedNodes[nodeData->id] = true;
            } else {
                scriptInstance.selectedNodes.clear();
                scriptInstance.selectedNodes[nodeData->id] = true;
            }
            nodeData->isDragging = true;
        }

        if (nodeData->isDragging) {
            if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                ImVec2 delta = ImGui::GetIO().MouseDelta;
                nodeData->x += delta.x;
                nodeData->y += delta.y;
            }
            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                nodeData->isDragging = false;
            }
        }
    }

    void CanvasPanel::renderNodes(ImVec2 pos) {
        for (auto &node: scriptInstance.nodes) {
            auto &nodeData = node.second;
            // Calculate node's screen position and size.
            ImVec2 nodePos = ImVec2(pos.x + nodeData->x, pos.y + nodeData->y + 50);
            ImVec2 nodeSize = ImVec2(nodeData->width, nodeData->height);
            ImVec2 nodeRectMin = nodePos;
            ImVec2 nodeRectMax = ImVec2(nodePos.x + nodeSize.x, nodePos.y + nodeSize.y);

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
            bool isSelected = scriptInstance.selectedNodes.contains(nodeData->id);
            if (isSelected) {
                drawList->AddText(textPos, IM_COL32(0, 128, 255, 255), nodeData->name.c_str());
            } else {
                drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), nodeData->name.c_str());
            }
            // Draw separator under the header.
            ImVec2 separatorStart = ImVec2(nodeRectMin.x, headerRectMax.y);
            ImVec2 separatorEnd = ImVec2(nodeRectMax.x, headerRectMax.y);
            drawList->AddLine(separatorStart, separatorEnd, IM_COL32(80, 80, 80, 255), 1.0f);

            // --- Handle Dragging on Header ---
            // Create an invisible button covering the header area.
            ImGui::SetCursorScreenPos(headerRectMin);
            std::string headerId = node.first + "_header";
            ImGui::InvisibleButton(headerId.c_str(), ImVec2(nodeSize.x, NODE_HEADER_SIZE));
            if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                ImVec2 delta = ImGui::GetIO().MouseDelta;
                nodeData->x += delta.x;
                nodeData->y += delta.y;
            }

            // --- Render IO Elements ---
            RenderNodeIO(nodeData.get(), nodePos, scriptInstance);
        }
    }

    void CanvasPanel::onSync() {
        onSyncChildren();
        ImGui::Separator();
        auto pos = ImGui::GetWindowPos();
        ImGui::PushStyleColor(ImGuiCol_ChildBg, context->themeService.palette4);
        renderNodes(pos);
        ImGui::PopStyleColor();
    }
} // Metal
