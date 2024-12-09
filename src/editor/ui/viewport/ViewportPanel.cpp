#include "ViewportPanel.h"

#include "../../../common/runtime/ApplicationContext.h"

namespace Metal {
    void ViewportPanel::onSync() {
        ImGui::Text("Viewport");
        ImGui::Image(reinterpret_cast<ImTextureID>(context->getEngineContext().descriptorSet), ImVec2(800, 600));
    }
}
