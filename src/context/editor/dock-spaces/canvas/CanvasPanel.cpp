#include "CanvasPanel.h"

#include "CanvasHeaderPanel.h"
#include "imgui.h"
#include "../../../ApplicationContext.h"
#include "../../../../dto/scripting/IO.h"
#define IO_CIRCLE_RADIUS 5.0f
#define IO_CIRCLE_RADIUS_2 10.f
#define IO_CIRCLE_RADIUS_2_5 IO_CIRCLE_RADIUS * 2.5f
#define NODE_HEADER_SIZE 30.f

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
                                   nodeWindowPos.y + io.offsetY * IO_CIRCLE_RADIUS_2_5 + NODE_HEADER_SIZE);
            } else {
                circlePos = ImVec2(nodeWindowPos.x + nodeData->width - IO_CIRCLE_RADIUS_2,
                                   nodeWindowPos.y + io.offsetY * IO_CIRCLE_RADIUS_2_5 + NODE_HEADER_SIZE);
            }

            ImGui::SetCursorScreenPos(ImVec2(circlePos.x - IO_CIRCLE_RADIUS, circlePos.y - IO_CIRCLE_RADIUS));
            ImGui::InvisibleButton(std::format("io_%d_%s", io.id, io.isInput ? "in" : "out").c_str(),
                                   ImVec2(IO_CIRCLE_RADIUS_2, IO_CIRCLE_RADIUS_2));
            bool ioHovered = ImGui::IsItemHovered();

            if (io.isInput && ioHovered && s_draggingOutput) {
                s_hoveredInput = &io;
            }

            ImU32 ioColor = GetColorForIOType(io.type);
            if (io.isInput && s_draggingOutput && ioHovered) {
                ioColor = IM_COL32(200, 200, 255, 255);
            }

            ImGui::GetWindowDrawList()->AddCircleFilled(circlePos, IO_CIRCLE_RADIUS, ioColor);
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
            ImVec2 nodePos = ImVec2(pos.x + nodeData->x, pos.y + nodeData->y + 50);
            ImGui::SetNextWindowPos(nodePos);
            ImGui::BeginChild(node.first.c_str(), ImVec2(nodeData->width, nodeData->height));

            bool isSelected = scriptInstance.selectedNodes.contains(nodeData->id);
            if (isSelected) {
                ImGui::TextColored(ImVec4(0, .5, 1, 1), nodeData->name.c_str());
            } else {
                ImGui::Text(nodeData->name.c_str());
            }
            ImGui::Separator();


            handleNodeDrag(nodeData);

            ImGui::EndChild();
            RenderNodeIO(nodeData.get(), nodePos, scriptInstance);
        }
    }

    void CanvasPanel::onSync() {
        onSyncChildren();
        ImGui::Separator();
        auto pos = ImGui::GetWindowPos();
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(.5, .5, .5, 1));

        renderNodes(pos);

        ImGui::PopStyleColor();
    }
} // Metal
