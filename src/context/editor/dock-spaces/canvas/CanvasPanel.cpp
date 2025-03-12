#include "CanvasPanel.h"

#include "components/CanvasHeaderPanel.h"
#include "imgui.h"
#include "../../../ApplicationContext.h"
#include "components/IOPanel.h"
#include "components/LinkPanel.h"
#include "components/NodePanel.h"

namespace Metal {
    void CanvasPanel::onInitialize() {
        appendChild(header = new CanvasHeaderPanel(&scriptInstance));
        appendChild(nodePanel = new NodePanel);
        appendChild(linkPanel = new LinkPanel);
        appendChild(ioPanel = new IOPanel);
    }

    void CanvasPanel::drawBackground() const {
        ImDrawList *draw_list = ImGui::GetWindowDrawList();
        ImVec2 win_pos = ImGui::GetWindowPos();
        ImVec2 win_size = ImGui::GetWindowSize();

        ImU32 color = IM_COL32(context->themeService.palette4.x * 255, context->themeService.palette4.y * 255,
                               context->themeService.palette4.z * 255, 255);
        for (float x = fmod(win_pos.x, spacing.x); x < win_pos.x + win_size.x; x += spacing.x)
            draw_list->AddLine(ImVec2(x, win_pos.y), ImVec2(x, win_pos.y + win_size.y), color);

        for (float y = fmod(win_pos.y, spacing.y); y < win_pos.y + win_size.y; y += spacing.y)
            draw_list->AddLine(ImVec2(win_pos.x, y), ImVec2(win_pos.x + win_size.x, y), color);
    }

    void CanvasPanel::renderNodes() {
        ioPanel->setScriptInstance(&scriptInstance);
        nodePanel->setScriptInstance(&scriptInstance);
        for (auto &node: scriptInstance.nodes) {
            auto &nodeData = node.second;
            nodePanel->setNode(&nodeData);
            nodePanel->onSync();
            ioPanel->setNode(&nodeData);
            ioPanel->onSync();
        }
        ioPanel->onAfterSync();
    }

    void CanvasPanel::renderLinks() {
        linkPanel->setScriptInstance(&scriptInstance);
        for (auto &link: scriptInstance.links) {
            linkPanel->setLink(&link.second);
            linkPanel->onSync();
        }
    }

    void CanvasPanel::renderVirtualCanvas() {
        ImGui::BeginChild("Canvas", ImVec2(10000, 10000), false, ImGuiWindowFlags_NoBackground);

        drawBackground();

        ImGui::PushStyleColor(ImGuiCol_ChildBg, context->themeService.palette4);
        renderNodes();
        renderLinks();
        ImGui::PopStyleColor();

        ImGui::EndChild(); // End virtual canvas
    }

    void CanvasPanel::handleCanvasDrag() {
        ImVec2 mouseDelta = ImGui::GetIO().MouseDelta;
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Right)) {
            canvasOffset.x += mouseDelta.x;
            canvasOffset.y += mouseDelta.y;
        }
    }

    void CanvasPanel::onSync() {
        header->onSync();
        ImGui::Separator();
        ImGui::BeginChild(("CanvasPanel" + id).c_str(),
                          ImGui::GetContentRegionAvail(),
                          true, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar |
                                ImGuiWindowFlags_NoScrollWithMouse);
        handleCanvasDrag();
        ImGui::SetCursorPos(canvasOffset);
        renderVirtualCanvas();
        ImGui::EndChild();
    }
} // Metal
