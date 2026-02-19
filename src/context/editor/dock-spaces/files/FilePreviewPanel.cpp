#include "FilePreviewPanel.h"
#include <imgui.h>
#include "../../../../util/UIUtil.h"
#include "../../../../dto/file/FileEntry.h"
#include "../../../../context/ApplicationContext.h"
#include "../../../../common/interface/Icons.h"
#include "../../../../service/texture/TextureInstance.h"
#include "../inspector/MaterialInspection.h"

namespace Metal {
    FilePreviewPanel::FilePreviewPanel(FilesContext &filesContext) : filesContext(filesContext) {
    }

    void FilePreviewPanel::onInitialize() {
        materialInspection = new MaterialInspection();
        appendChild(materialInspection);
    }

    void FilePreviewPanel::onSync() {
        if (filesContext.selected.empty()) {
            ImGui::Text("Select a file to preview");
            return;
        }

        FileEntry *selected = filesContext.selected.begin()->second;

        ImGui::Text(selected->name.c_str());
        ImGui::Separator();

        if (selected->type == EntryType::TEXTURE) {
            auto *texture = ApplicationContext::Get().streamingRepository.streamTexture(selected->getId(), LevelOfDetail::LOD_3);
            if (texture != nullptr) {
                float availWidth = ImGui::GetContentRegionAvail().x;
                float availHeight = ImGui::GetContentRegionAvail().y;

                float imgWidth = (float)texture->width;
                float imgHeight = (float)texture->height;
                float aspectRatio = imgWidth / imgHeight;

                float renderWidth = availWidth;
                float renderHeight = renderWidth / aspectRatio;

                if (renderHeight > availHeight) {
                    renderHeight = availHeight;
                    renderWidth = renderHeight * aspectRatio;
                }

                // Center the image in the remaining space
                float offsetX = (availWidth - renderWidth) * 0.5f;
                if (offsetX > 0) ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);

                ApplicationContext::Get().guiContext.renderImage(texture, renderWidth, renderHeight);
            }
        } else if (selected->type == EntryType::MATERIAL) {
            ApplicationContext::Get().fileInspection.materialId = selected->getId();
            materialInspection->onSync();
        } else {
            ImGui::Text("File has no preview");
        }
    }
}
