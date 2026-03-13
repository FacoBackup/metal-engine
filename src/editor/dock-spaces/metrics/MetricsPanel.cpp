#include "MetricsPanel.h"
#include "../../../ApplicationContext.h"
#include "imgui.h"

namespace Metal {
    template<typename T>
    void drawResourceList(const char *label, AbstractResourceService<T> &service, std::string id) {
        auto &resources = service.getResources();
        if (ImGui::BeginChild((label + id).c_str())) {
            ImGui::Text("%s (%i)", label, resources.size());
            ImGui::Separator();
            if (ImGui::BeginTable(label, 1, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                for (const auto &[id, resource]: resources) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%s", id.c_str());
                }
                ImGui::EndTable();
            }
        }
        ImGui::EndChild();
    }

    void MetricsPanel::onSync() {
        drawResourceList("Meshes", applicationContext->meshService, id);
        drawResourceList("Voxels", applicationContext->voxelService, id);
        drawResourceList("Framebuffers", applicationContext->framebufferService, id);
        drawResourceList("Textures", applicationContext->textureService, id);
        drawResourceList("Pipelines", applicationContext->pipelineService, id);
        drawResourceList("Buffers", applicationContext->bufferService, id);
    }
}
