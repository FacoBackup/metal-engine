#include "CanvasHeaderPanel.h"

#include "../../../../dto/scripting/IO.h"
#include "../../../../dto/scripting/ScriptInstance.h"
#include "../../../../util/UIUtil.h"

namespace Metal {
    CanvasHeaderPanel::CanvasHeaderPanel(ScriptInstance *script) {
        this->script = script;
    }

    void CanvasHeaderPanel::onSync() {
        ImGui::SetNextItemWidth(50);
        if (ImGui::BeginMenu(("Add node" + id).c_str())) {
            if (ImGui::MenuItem("And")) {
                script->addNode(AND);
            }
            if (ImGui::MenuItem("Boolean")) {
                script->addNode(NodeType::BOOL);
            }
            ImGui::EndMenu();
        }
    }
} // Metal