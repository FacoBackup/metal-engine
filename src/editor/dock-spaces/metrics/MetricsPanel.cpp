#include "MetricsPanel.h"
#include "../../../ApplicationContext.h"
#include "imgui.h"
#include "../../../engine/service/MeshService.h"
#include "../../../engine/service/VoxelService.h"
#include "../../../engine/service/FrameBufferService.h"
#include "../../../engine/service/TextureService.h"
#include "../../../engine/service/PipelineService.h"
#include "../../../engine/service/BufferService.h"

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
        if (meshService) drawResourceList("Meshes", *meshService, id);
        if (voxelService) drawResourceList("Voxels", *voxelService, id);
        if (framebufferService) drawResourceList("Framebuffers", *framebufferService, id);
        if (textureService) drawResourceList("Textures", *textureService, id);
        if (pipelineService) drawResourceList("Pipelines", *pipelineService, id);
        if (bufferService) drawResourceList("Buffers", *bufferService, id);
    }
}
