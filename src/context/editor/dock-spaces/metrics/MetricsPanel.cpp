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
        drawResourceList("Meshes", CTX.meshService, id);
        drawResourceList("Materials", CTX.materialService, id);
        drawResourceList("Voxels", CTX.voxelService, id);
        drawResourceList("Framebuffers", CTX.framebufferService, id);
        drawResourceList("Textures", CTX.textureService, id);
        drawResourceList("Pipelines", CTX.pipelineService, id);
        drawResourceList("Buffers", CTX.bufferService, id);
    }
}
