#include "FilesHeader.h"

#include "../../../common/interface/Icons.h"
#include "../../../common/util/files/FileDialogUtil.h"
#include "../../../common/util/files/FilesUtil.h"
#include "../../../context/ApplicationContext.h"
#include "../../common/UIUtil.h"

namespace Metal {
    void FilesHeader::onSync() {
        if (UIUtil::ButtonSimple(Icons::create_new_folder + id, UIUtil::ONLY_ICON_BUTTON_SIZE,
                                 UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            FilesUtil::MkDir(
                filesContext->currentDirectory->absolutePath + "/new-directory (" + Util::uuidV4().substr(0, 4) + ")");
            FilesUtil::GetEntries(*filesContext->currentDirectory);
        }
        ImGui::SameLine();

        if (UIUtil::ButtonSimple(Icons::arrow_upward, UIUtil::ONLY_ICON_BUTTON_SIZE, UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            filesContext->setCurrentDirectory(filesContext->currentDirectory->parent);
        }
        ImGui::SameLine();
        ImGui::Text(filesContext->pathToCurrentDirectory.c_str());
        UIUtil::DynamicSpacing(105);
        if (UIUtil::ButtonSimple(Icons::file_open + "Import file" + id, 100, UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            auto files = FileDialogUtil::PickFiles({{"Mesh", "fbx,gltf,obj,glb"}, {"Image", "png,jpg,jpeg"}});
            for (const std::string &file: files) {
                if (context->getEditorContext().meshImporter.isCompatible(file)) {
                    context->getEditorContext().meshImporter.importMesh(file);
                }
            }
        }
    }
} // Metal
