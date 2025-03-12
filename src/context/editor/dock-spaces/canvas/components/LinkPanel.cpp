#include "LinkPanel.h"

#include <imgui.h>

#include "../../../../../dto/scripting/Link.h"
#include "../../../../../dto/scripting/ScriptInstance.h"
#include "../CanvasUtil.h"

namespace Metal {
    void LinkPanel::onSync() {
        auto &sourceNode = scriptInstance->nodes.at(link->sourceNodeId);
        auto &targetNode = scriptInstance->nodes.at(link->targetNodeId);
        auto &sourceIo = sourceNode.ioMap.at(link->source);
        ImVec2 linkSource = sourceIo.circlePos;
        ImVec2 linkTarget = targetNode.ioMap.at(link->target).circlePos;
        ImGui::GetWindowDrawList()->AddBezierCubic(
            linkSource,
            ImVec2(linkSource.x + 50, linkSource.y),
            ImVec2(linkTarget.x - 50, linkTarget.y),
            linkTarget,
            CanvasUtil::GetColorForIOType(sourceIo.type),
            2.0f);
    }
} // Metal
