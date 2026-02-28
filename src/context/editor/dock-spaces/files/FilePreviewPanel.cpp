#include "FilePreviewPanel.h"
#include <imgui.h>
#include "../../../../util/UIUtil.h"
#include "../../../../dto/file/FSEntry.h"
#include "../../../../context/ApplicationContext.h"
#include "../../../../common/interface/Icons.h"
#include "../../../../service/texture/TextureInstance.h"
#include "../inspector/MaterialEditPanel.h"

namespace Metal {
    FilePreviewPanel::FilePreviewPanel(FilesContext &filesContext) : filesContext(filesContext) {
    }

    void FilePreviewPanel::onInitialize() {
        materialInspection = new MaterialEditPanel();
        appendChild(materialInspection);
    }

    void FilePreviewPanel::onSync() {
        if (filesContext.selected.empty()) {
            ImGui::Text("Select a file to preview");
            return;
        }

        FSEntry *selected = filesContext.selected.begin()->second;

        ImGui::Text(selected->name.c_str());
        ImGui::Separator();

        if (selected->type == EntryType::TEXTURE) {
            auto *texture = CTX.streamingRepository.streamTexture(selected->getId());
            if (texture != nullptr) {
                float availWidth = ImGui::GetContentRegionAvail().x;
                float availHeight = ImGui::GetContentRegionAvail().y * 0.6f; // reserve space for table

                float imgWidth = (float)texture->width;
                float imgHeight = (float)texture->height;
                float aspectRatio = imgWidth / imgHeight;

                float renderWidth = availWidth;
                float renderHeight = renderWidth / aspectRatio;

                if (renderHeight > availHeight) {
                    renderHeight = availHeight;
                    renderWidth = renderHeight * aspectRatio;
                }

                float offsetX = (availWidth - renderWidth) * 0.5f;
                if (offsetX > 0) ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);

                CTX.guiContext.renderImage(texture, renderWidth, renderHeight);
                ImGui::Separator();
            }
        } else if (selected->type == EntryType::MATERIAL) {
            CTX.fileInspection.materialId = selected->getId();
            materialInspection->onSync();
            ImGui::Separator();
        }

        if (ImGui::BeginTable((id + "metadata").c_str(), 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            ImGui::TableSetupColumn("Property");
            ImGui::TableSetupColumn("Value");
            ImGui::TableHeadersRow();

            ImGui::TableNextRow();
            ImGui::TableNextColumn(); ImGui::Text("Name");
            ImGui::TableNextColumn(); ImGui::Text("%s", selected->name.c_str());

            ImGui::TableNextRow();
            ImGui::TableNextColumn(); ImGui::Text("ID");
            ImGui::TableNextColumn(); ImGui::Text("%s", selected->getId().c_str());

            const char *typeLabel = "";
            switch (selected->type) {
                case EntryType::SCENE: typeLabel = "Scene"; break;
                case EntryType::MESH: typeLabel = "Mesh"; break;
                case EntryType::TEXTURE: typeLabel = "Texture"; break;
                case EntryType::VOLUME: typeLabel = "Volume"; break;
                case EntryType::MATERIAL: typeLabel = "Material"; break;
                case EntryType::DIRECTORY: typeLabel = "Directory"; break;
                default: typeLabel = "Unknown"; break;
            }
            ImGui::TableNextRow();
            ImGui::TableNextColumn(); ImGui::Text("Type");
            ImGui::TableNextColumn(); ImGui::Text("%s", typeLabel);

            ImGui::TableNextRow();
            ImGui::TableNextColumn(); ImGui::Text("Size");
            ImGui::TableNextColumn(); ImGui::Text("%s", selected->formattedSize.c_str());

            ImGui::TableNextRow();
            ImGui::TableNextColumn(); ImGui::Text("Date");
            ImGui::TableNextColumn(); ImGui::Text("%s", selected->formattedDate.c_str());

            ImGui::EndTable();
        }
    }
}
